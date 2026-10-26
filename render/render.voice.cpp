// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/audio.hpp>
#include <threads.hpp>

#include "../graph.hpp"
#include "render.hpp"
#include "render.plan.hpp"
#include "render.voice.hpp"
#include "../transport.hpp"

namespace SOUND::RENDER::VOICE {
namespace {

Whole voice = AUDIO::OUTPUT::NONE;
String clock;
Whole width = CHANNELS;
Whole lane = 0;
THREADS::Shared<Whole> begun{0};
THREADS::Shared<Whole> ended{0};
THREADS::Shared<Whole> torn{0};
THREADS::Shared<Whole> underruns{0};

auto quantized(Float held) -> AUDIO::Sample {
  return static_cast<AUDIO::Sample>(
    std::clamp(held, -1.0f, 1.0f) * (SCALE - 1.0f));
}

auto walked(AUDIO::Sample *into, Whole made, Whole run) -> Flag {
  TRANSPORT::advance(run);
  const TRANSPORT::Statement &want = TRANSPORT::block();
  const Span cycle = want.looping ? Span{want.from, want.to} : Span{};
  const RENDER::Walk walk =
    RENDER::walk(want.playing ? TRANSPORT::started() : NONE, run, cycle);
  if (!RENDER::block(walk, run)) return false;
  const Vector<Vector<AUDIO::PLUGIN::Sample>> &lanes = RENDER::sounded(clock);
  if (lanes.size() < CHANNELS) return false;
  for (Whole frame = 0; frame < run; ++frame) {
    for (Whole at = 0; at < width; ++at) into[(made + frame) * width + at] = 0;
    for (Whole channel = 0; channel < CHANNELS; ++channel)
      into[(made + frame) * width + std::min(lane + channel, width - 1)] =
        quantized(lanes[channel][frame]);
  }
  return true;
}

auto counted(AUDIO::Sample *into, Whole made, Whole run) -> Flag {
  begun.fetch_add(1);
  GRAPH::SIGNAL::clean();
  const Whole head = GRAPH::SIGNAL::counted();
  const Flag walk = walked(into, made, run);
  if (GRAPH::SIGNAL::touched() && GRAPH::SIGNAL::counted() != head)
    torn.fetch_add(1);
  ended.fetch_add(1);
  return walk;
}

auto pulled(AUDIO::Sample *into, Whole frames, void *) -> Whole {
  Whole made = 0;
  while (made < frames) {
    const Whole run = std::min<Whole>(frames - made, RENDER::BLOCK);
    if (!counted(into, made, run)) break;
    made += run;
  }
  if (made < frames) underruns.fetch_add(1);
  return made;
}

}  // namespace
}  // namespace SOUND::RENDER::VOICE

auto SOUND::RENDER::VOICE::blocks() -> Blocks {
  return {begun.load(), ended.load(), torn.load(), underruns.load()};
}

auto SOUND::RENDER::VOICE::standing() -> Flag {
  return voice != AUDIO::OUTPUT::NONE;
}

void SOUND::RENDER::VOICE::claim() {
  if (standing()) return;
  RENDER::plan();
  clock = GRAPH::clock();
  if (clock.empty()) return;
  const Node &node = GRAPH::held().nodes[GRAPH::at(clock)];
  width = CHANNELS;
  for (const AUDIO::OUTPUT::Device &row : AUDIO::OUTPUT::GET::devices())
    if (!node.device.empty() && row.name == node.device && row.channels != 0)
      width = std::min(row.channels, AUDIO::LANES);
  lane = std::min(node.lane, width - 1);
  voice = AUDIO::OUTPUT::create(RATE, width, node.device, pulled, nullptr);
}

void SOUND::RENDER::VOICE::drop() {
  if (voice == AUDIO::OUTPUT::NONE) return;
  AUDIO::OUTPUT::remove(voice);
  voice = AUDIO::OUTPUT::NONE;
  TRANSPORT::detach();
}

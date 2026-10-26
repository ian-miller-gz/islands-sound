// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/audio.hpp>

#include "../graph.hpp"
#include "render.hpp"
#include "../transport.hpp"

namespace SOUND::RENDER::VOICE {
namespace {

Whole voice = AUDIO::OUTPUT::NONE;
String clock;
Whole width = CHANNELS;
Whole lane = 0;

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

auto pulled(AUDIO::Sample *into, Whole frames, void *) -> Whole {
  Whole made = 0;
  while (made < frames) {
    const Whole run = std::min<Whole>(frames - made, RENDER::BLOCK);
    if (!walked(into, made, run)) return made;
    made += run;
  }
  return made;
}

}  // namespace
}  // namespace SOUND::RENDER::VOICE

void SOUND::RENDER::VOICE::claim() {
  if (voice != AUDIO::OUTPUT::NONE) return;
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

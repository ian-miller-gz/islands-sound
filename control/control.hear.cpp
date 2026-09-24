// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "control.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole ROOM = 1024;

auto wide(const String &device) -> Whole {
  for (const AUDIO::INPUT::Device &row : CONTROL::inputs())
    if (row.name == device && row.channels != 0)
      return std::min(row.channels, AUDIO::LANES);
  return CHANNELS;
}

void append(CONTROL::Live &live, const AUDIO::Sample *from, Whole frames) {
  live.take.heard.resize(CHANNELS);
  for (Whole frame = 0; frame < frames; ++frame)
    for (Whole channel = 0; channel < CHANNELS; ++channel) {
      const Whole lane = std::min(live.picked + channel, live.lanes - 1);
      live.take.heard[channel].push_back(
        Float(from[frame * live.lanes + lane]) / SCALE);
    }
}

}  // namespace

void SOUND::CONTROL::listening(Flag on) {
  Live &live = standing();
  if (on == (live.stream != AUDIO::INPUT::NONE)) return;
  if (!on) {
    AUDIO::INPUT::remove(live.stream);
    live.stream = AUDIO::INPUT::NONE;
    live.lanes = CHANNELS;
    live.picked = 0;
    return;
  }
  const String device =
    offered(live.input.device) ? live.input.device : String();
  const Whole lanes = ::wide(device);
  live.stream = AUDIO::INPUT::create(RATE, lanes, device);
  if (live.stream == AUDIO::INPUT::NONE) return;
  live.lanes = lanes;
  live.picked = std::min(live.input.channel, lanes - 1);
  live.scratch.assign(::ROOM * lanes, 0);
}

auto SOUND::CONTROL::hear() -> Whole {
  Live &live = standing();
  if (live.stream == AUDIO::INPUT::NONE) return 0;
  Whole taken = 0;
  for (Whole frames = 1; frames != 0;) {
    frames = AUDIO::INPUT::read(live.stream, live.scratch);
    taken += hear(live.scratch.data(), frames);
    if (frames * live.lanes < live.scratch.size()) return taken;
  }
  return taken;
}

auto SOUND::CONTROL::hear(const AUDIO::Sample *from, Whole frames) -> Whole {
  Live &live = standing();
  if (!live.taking || frames == 0) return 0;
  ::append(live, from, frames);
  return frames;
}

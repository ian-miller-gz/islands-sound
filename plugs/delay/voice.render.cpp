// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

auto sounded(const DELAY::Trail &trail, DELAY::Lane &lane, Float came)
  -> Float {
  const Whole back = lane.at >= trail.spacing
                       ? lane.at - trail.spacing
                       : lane.at + trail.slots - trail.spacing;
  const Float taken = lane.ring[back];
  lane.ring[lane.at] = came + taken * trail.given;
  lane.at = lane.at + 1 == trail.slots ? 0 : lane.at + 1;
  return came * trail.dry + taken * trail.wet;
}

void publish(DELAY::Trail &trail, Float peak) {
  const Whole idle = trail.face.load(std::memory_order_relaxed) ^ 1u;
  trail.peak[idle] = peak;
  trail.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::DELAY::render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &trail = *static_cast<Trail *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(trail, events[next++]);
    for (Whole channel = 0; channel < trail.channels; ++channel) {
      const Float value =
        ::sounded(trail, trail.lanes[channel], lanes[channel][frame]);
      lanes[channel][frame] = value;
      const Float size = value < 0 ? -value : value;
      if (size > peak) peak = size;
    }
  }
  while (next < count) apply(trail, events[next++]);
  ::publish(trail, peak);
}

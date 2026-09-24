// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

constexpr Float PAIR = 2.0f;

auto sifted(const FILTER::Sieve &sieve, FILTER::Lane &lane, Float value)
  -> Float {
  const Float across = value - lane.low;
  const Float ringing = sieve.loop * lane.band + sieve.feed * across;
  const Float below = lane.low + sieve.feed * lane.band + sieve.carry * across;
  lane.band = PAIR * ringing - lane.band;
  lane.low = PAIR * below - lane.low;
  switch (sieve.mode) {
    case FILTER::HIGH:
      return value - sieve.damping * ringing - below;
    case FILTER::BAND:
      return ringing;
    default:
      return below;
  }
}

void publish(FILTER::Sieve &sieve, Float peak) {
  const Whole idle = sieve.face.load(std::memory_order_relaxed) ^ 1u;
  sieve.peak[idle] = peak;
  sieve.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::FILTER::render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &sieve = *static_cast<Sieve *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(sieve, events[next++]);
    for (Whole channel = 0; channel < sieve.channels; ++channel) {
      const Float value =
        ::sifted(sieve, sieve.lanes[channel], lanes[channel][frame]);
      lanes[channel][frame] = value;
      const Float magnitude = value < 0 ? -value : value;
      if (magnitude > peak) peak = magnitude;
    }
  }
  while (next < count) apply(sieve, events[next++]);
  ::publish(sieve, peak);
}

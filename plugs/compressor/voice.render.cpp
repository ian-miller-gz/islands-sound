// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

constexpr Float WHOLE = 1.0f;

auto loudest(AUDIO::PLUGIN::Sample *const *lanes, Whole channels, Whole frame)
  -> Float {
  Float heard = 0;
  for (Whole channel = 0; channel < channels; ++channel) {
    const Float value = lanes[channel][frame];
    const Float size = value < 0 ? -value : value;
    if (size > heard) heard = size;
  }
  return heard;
}

auto squeezed(const COMPRESSOR::Press &press) -> Float {
  if (press.envelope <= press.threshold) return WHOLE;
  const Float over = press.threshold / press.envelope;
  if (press.folds == 0) return over;
  Float root = over;
  Float given = WHOLE;
  for (Whole fold = 0; fold < press.folds; ++fold) {
    root = std::sqrt(root);
    given *= root;
  }
  return given;
}

void publish(COMPRESSOR::Press &press, Float peak) {
  const Whole idle = press.face.load(std::memory_order_relaxed) ^ 1u;
  press.peak[idle] = peak;
  press.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::COMPRESSOR::render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &press = *static_cast<Press *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(press, events[next++]);
    const Float heard = ::loudest(lanes, press.channels, frame);
    const Float step = heard > press.envelope ? press.rising : press.falling;
    press.envelope += (heard - press.envelope) * step;
    const Float gain = ::squeezed(press) * press.makeup;
    for (Whole channel = 0; channel < press.channels; ++channel) {
      const Float value = lanes[channel][frame] * gain;
      lanes[channel][frame] = value;
      const Float size = value < 0 ? -value : value;
      if (size > peak) peak = size;
    }
  }
  while (next < count) apply(press, events[next++]);
  ::publish(press, peak);
}

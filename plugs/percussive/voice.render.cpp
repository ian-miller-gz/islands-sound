// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float SCALE = 1.0f / 2147483648.0f;
constexpr uint32_t SPIN = 1664525u;
constexpr uint32_t SKEW = 1013904223u;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

auto sound(
  const PERCUSSIVE::Machine &machine, PERCUSSIVE::Strike &strike,
  Whole slot) -> Float {
  if (strike.level <= 0) return 0;
  const PERCUSSIVE::Recipe &made = PERCUSSIVE::recipe(slot);
  Float body = 0;
  if (made.tone > 0) {
    body = machine.wave[strike.phase >> PERCUSSIVE::SHIFT];
    strike.phase += uint32_t(strike.step);
    strike.step -= strike.bend;
    if (strike.step < strike.rest) strike.step = strike.rest;
  }
  strike.noise = strike.noise * SPIN + SKEW;
  const Float raw = Float(static_cast<int32_t>(strike.noise)) * SCALE;
  strike.held += made.rasp * (raw - strike.held);
  const Float grit = raw - strike.held;
  const Float mix = body * made.tone + grit * (1.0f - made.tone);
  strike.level -= strike.drop;
  if (strike.level < 0) strike.level = 0;
  return mix * strike.level * strike.velocity * machine.levels[slot];
}

void publish(PERCUSSIVE::Machine &machine, Float peak) {
  const Whole idle = machine.face.load(std::memory_order_relaxed) ^ 1u;
  machine.level[idle] = peak;
  machine.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::PERCUSSIVE::render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &machine = *static_cast<Machine *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(machine, events[next++]);
    Float sample = 0;
    for (Whole slot = 0; slot < SLOTS; ++slot)
      sample += ::sound(machine, machine.strikes[slot], slot);
    sample *= machine.gain;
    for (Whole channel = 0; channel < machine.channels; ++channel)
      outputs[channel][frame] = sample;
    const Float magnitude = sample < 0 ? -sample : sample;
    if (magnitude > peak) peak = magnitude;
  }
  while (next < count) apply(machine, events[next++]);
  ::publish(machine, peak);
}

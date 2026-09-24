// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float SCALE = 1.0f / 2147483648.0f;
constexpr Float PAIR = 0.5f;
constexpr Float CEILING = 0.45f;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

void breathe(const SUBTRACTIVE::Envelope &envelope, SUBTRACTIVE::Gate &gate) {
  switch (gate.stage) {
    case SUBTRACTIVE::RISING:
      gate.level += envelope.rise;
      if (gate.level < 1.0f) return;
      gate.level = 1.0f;
      gate.stage = SUBTRACTIVE::FALLING;
      return;
    case SUBTRACTIVE::FALLING:
      gate.level -= envelope.fall;
      if (envelope.fall > 0 && gate.level > envelope.sustain) return;
      gate.level = envelope.sustain;
      gate.stage = SUBTRACTIVE::HELD;
      return;
    case SUBTRACTIVE::HELD:
      gate.level = envelope.sustain;
      return;
    case SUBTRACTIVE::LEAVING:
      gate.level -= envelope.drop;
      if (gate.level > 0) return;
      gate.level = 0;
      gate.stage = SUBTRACTIVE::IDLE;
      return;
    default:
      return;
  }
}

auto wave(Whole shape, uint32_t phase) -> Float {
  const Float ramp = Float(static_cast<int32_t>(phase)) * SCALE;
  switch (shape) {
    case SUBTRACTIVE::PULSE:
      return ramp < 0 ? -1.0f : 1.0f;
    case SUBTRACTIVE::TRIANGLE:
      return (ramp < 0 ? -ramp : ramp) * 2.0f - 1.0f;
    default:
      return ramp;
  }
}

auto sound(const SUBTRACTIVE::Synth &synth, SUBTRACTIVE::Note &note) -> Float {
  if (note.gates[SUBTRACTIVE::LOUDNESS].stage == SUBTRACTIVE::IDLE) return 0;
  Float mix = 0;
  for (Whole voice = 0; voice < SUBTRACTIVE::OSCILLATORS; ++voice) {
    mix += ::wave(synth.shape, note.phases[voice]);
    note.phases[voice] += note.steps[voice];
  }
  mix *= PAIR;
  const Float lift = note.gates[SUBTRACTIVE::CONTOUR].level * synth.depth;
  Float feed = (synth.cutoff + lift) * synth.turn;
  if (feed > CEILING) feed = CEILING;
  note.band += feed * (mix - note.low - synth.damping * note.band);
  note.low += feed * note.band;
  return note.low * note.gates[SUBTRACTIVE::LOUDNESS].level * note.velocity;
}

void publish(SUBTRACTIVE::Synth &synth, Float peak) {
  const Whole idle = synth.face.load(std::memory_order_relaxed) ^ 1u;
  synth.level[idle] = peak;
  synth.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::SUBTRACTIVE::render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &synth = *static_cast<Synth *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(synth, events[next++]);
    Float sample = 0;
    for (Note &note : synth.notes) {
      for (Whole curve = 0; curve < CURVES; ++curve)
        ::breathe(synth.envelopes[curve], note.gates[curve]);
      sample += ::sound(synth, note);
    }
    sample *= synth.gain;
    for (Whole channel = 0; channel < synth.channels; ++channel)
      outputs[channel][frame] = sample;
    const Float magnitude = sample < 0 ? -sample : sample;
    if (magnitude > peak) peak = magnitude;
  }
  while (next < count) apply(synth, events[next++]);
  ::publish(synth, peak);
}

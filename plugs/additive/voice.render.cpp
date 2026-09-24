// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

void breathe(const ADDITIVE::Synth &synth, ADDITIVE::Note &note) {
  switch (note.stage) {
    case ADDITIVE::RISING:
      note.level += synth.rise;
      if (note.level < 1.0f) return;
      note.level = 1.0f;
      note.stage = ADDITIVE::FALLING;
      return;
    case ADDITIVE::FALLING:
      note.level -= synth.fall;
      if (synth.fall > 0 && note.level > synth.sustain) return;
      note.level = synth.sustain;
      note.stage = ADDITIVE::HELD;
      return;
    case ADDITIVE::HELD:
      note.level = synth.sustain;
      return;
    case ADDITIVE::LEAVING:
      note.level -= synth.drop;
      if (note.level > 0) return;
      note.level = 0;
      note.stage = ADDITIVE::IDLE;
      return;
    default:
      return;
  }
}

auto sound(const ADDITIVE::Synth &synth, ADDITIVE::Note &note) -> Float {
  if (note.stage == ADDITIVE::IDLE) return 0;
  Float mix = 0;
  for (Whole partial = 0; partial < ADDITIVE::PARTIALS; ++partial) {
    const Float amplitude = synth.amplitudes[partial];
    if (amplitude == 0) continue;
    const uint32_t phase = note.phase * static_cast<uint32_t>(partial + 1);
    mix += amplitude * synth.table[phase >> ADDITIVE::TURN];
  }
  note.phase += note.step;
  return mix / synth.span * note.level * note.velocity;
}

void publish(ADDITIVE::Synth &synth, Float peak) {
  const Whole idle = synth.face.load(std::memory_order_relaxed) ^ 1u;
  synth.level[idle] = peak;
  synth.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::ADDITIVE::render(
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
      ::breathe(synth, note);
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

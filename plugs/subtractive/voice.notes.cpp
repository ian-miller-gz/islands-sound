// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto claim(SUBTRACTIVE::Synth &synth) -> SUBTRACTIVE::Note & {
  SUBTRACTIVE::Note *oldest = &synth.notes[0];
  for (SUBTRACTIVE::Note &note : synth.notes) {
    if (note.gates[SUBTRACTIVE::LOUDNESS].stage == SUBTRACTIVE::IDLE)
      return note;
    if (note.struck < oldest->struck) oldest = &note;
  }
  return *oldest;
}

void strike(SUBTRACTIVE::Synth &synth, const AUDIO::PLUGIN::Event &event) {
  SUBTRACTIVE::Note &note = claim(synth);
  note.pitch = event.index < synth.steps.size() ? event.index : 0;
  note.steps[0] = synth.steps[note.pitch];
  note.steps[1] = uint32_t(Float(note.steps[0]) * synth.detune);
  note.velocity = event.value < 0 ? 0 : event.value > 1 ? 1 : event.value;
  for (uint32_t &phase : note.phases) phase = 0;
  note.low = 0;
  note.band = 0;
  for (SUBTRACTIVE::Gate &gate : note.gates) gate = {0, SUBTRACTIVE::RISING};
  note.struck = ++synth.clock;
}

void lift(SUBTRACTIVE::Synth &synth, Whole pitch) {
  for (SUBTRACTIVE::Note &note : synth.notes)
    if (note.pitch == pitch)
      for (SUBTRACTIVE::Gate &gate : note.gates)
        if (gate.stage != SUBTRACTIVE::IDLE) gate.stage = SUBTRACTIVE::LEAVING;
}

}  // namespace

void SOUND::SUBTRACTIVE::apply(
  Synth &synth, const AUDIO::PLUGIN::Event &event) {
  switch (event.kind) {
    case AUDIO::PLUGIN::Event::NOTE_ON:
      if (event.value > 0)
        ::strike(synth, event);
      else
        ::lift(synth, event.index);
      break;
    case AUDIO::PLUGIN::Event::NOTE_OFF:
      ::lift(synth, event.index);
      break;
    case AUDIO::PLUGIN::Event::CONTROLLER:
      steer(synth, event.index, event.value);
      break;
    default:
      break;
  }
}

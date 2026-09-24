// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float LOOSE = 2.0f;
constexpr Float SQUEEZE = 2.0f;

auto ratio(const SUBTRACTIVE::Synth &synth, Float cents) -> Float {
  const Whole whole = Whole(cents);
  if (whole >= SUBTRACTIVE::CENTS) return synth.ratios[SUBTRACTIVE::CENTS];
  const Float part = cents - Float(whole);
  const Float step = synth.ratios[whole + 1] - synth.ratios[whole];
  return synth.ratios[whole] + step * part;
}

auto tone(SUBTRACTIVE::Synth &synth, Whole id, Float held) -> Flag {
  switch (id) {
    case SUBTRACTIVE::GAIN:
      synth.gain = held;
      return true;
    case SUBTRACTIVE::SHAPE:
      synth.shape = Whole(held);
      return true;
    case SUBTRACTIVE::SPREAD:
      synth.spread = held;
      synth.detune = ratio(synth, held);
      return true;
    case SUBTRACTIVE::CUTOFF:
      synth.cutoff = held;
      return true;
    case SUBTRACTIVE::RESONANCE:
      synth.resonance = held;
      synth.damping = LOOSE - held * SQUEEZE;
      return true;
    case SUBTRACTIVE::DEPTH:
      synth.depth = held;
      return true;
    default:
      return false;
  }
}

auto curved(SUBTRACTIVE::Synth &synth, Whole id, Float held) -> Whole {
  SUBTRACTIVE::Envelope &contour = synth.envelopes[SUBTRACTIVE::CONTOUR];
  SUBTRACTIVE::Envelope &loudness = synth.envelopes[SUBTRACTIVE::LOUDNESS];
  switch (id) {
    case SUBTRACTIVE::SWEEP:
      contour.attack = held;
      return SUBTRACTIVE::CONTOUR;
    case SUBTRACTIVE::CLOSE:
      contour.decay = held;
      contour.release = held;
      return SUBTRACTIVE::CONTOUR;
    case SUBTRACTIVE::ATTACK:
      loudness.attack = held;
      return SUBTRACTIVE::LOUDNESS;
    case SUBTRACTIVE::DECAY:
      loudness.decay = held;
      return SUBTRACTIVE::LOUDNESS;
    case SUBTRACTIVE::SUSTAIN:
      loudness.sustain = held;
      return SUBTRACTIVE::LOUDNESS;
    case SUBTRACTIVE::RELEASE:
      loudness.release = held;
      return SUBTRACTIVE::LOUDNESS;
    default:
      return SUBTRACTIVE::CURVES;
  }
}

}  // namespace

void SOUND::SUBTRACTIVE::steer(Synth &synth, Whole id, Float value) {
  const Float held = clamped(id, value);
  if (::tone(synth, id, held)) return;
  const Whole curve = ::curved(synth, id, held);
  if (curve < CURVES) shape(synth.envelopes[curve], synth.rate);
}

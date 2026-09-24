// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto quantity(const SUBTRACTIVE::Synth &synth, Whole index) -> Float {
  const SUBTRACTIVE::Envelope &contour = synth.envelopes[SUBTRACTIVE::CONTOUR];
  const SUBTRACTIVE::Envelope &loudness =
    synth.envelopes[SUBTRACTIVE::LOUDNESS];
  switch (index) {
    case SUBTRACTIVE::GAIN:
      return synth.gain;
    case SUBTRACTIVE::SHAPE:
      return Float(synth.shape);
    case SUBTRACTIVE::SPREAD:
      return synth.spread;
    case SUBTRACTIVE::CUTOFF:
      return synth.cutoff;
    case SUBTRACTIVE::RESONANCE:
      return synth.resonance;
    case SUBTRACTIVE::DEPTH:
      return synth.depth;
    case SUBTRACTIVE::SWEEP:
      return contour.attack;
    case SUBTRACTIVE::CLOSE:
      return contour.decay;
    case SUBTRACTIVE::ATTACK:
      return loudness.attack;
    case SUBTRACTIVE::DECAY:
      return loudness.decay;
    case SUBTRACTIVE::SUSTAIN:
      return loudness.sustain;
    case SUBTRACTIVE::RELEASE:
      return loudness.release;
    default:
      return 0;
  }
}

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < SUBTRACTIVE::PARAMETERS;
}

}  // namespace

auto SOUND::SUBTRACTIVE::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::SUBTRACTIVE::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::SUBTRACTIVE::SURFACE::reading(void *instance, Whole index)
  -> String {
  if (!::sane(instance, index)) return {};
  return notation(index, held(instance, index));
}

auto SOUND::SUBTRACTIVE::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return ::quantity(*static_cast<const Synth *>(instance), index);
}

auto SOUND::SUBTRACTIVE::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  return ::sane(instance, index) && SUBTRACTIVE::control(index, out);
}

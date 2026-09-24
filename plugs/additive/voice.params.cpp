// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto quantity(const ADDITIVE::Synth &synth, Whole index) -> Float {
  switch (index) {
    case ADDITIVE::GAIN:
      return synth.gain;
    case ADDITIVE::ATTACK:
      return synth.attack;
    case ADDITIVE::DECAY:
      return synth.decay;
    case ADDITIVE::SUSTAIN:
      return synth.sustain;
    case ADDITIVE::RELEASE:
      return synth.release;
    default:
      break;
  }
  if (index < ADDITIVE::PARTIAL || index >= ADDITIVE::PARAMETERS) return 0;
  return synth.amplitudes[index - ADDITIVE::PARTIAL];
}

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < ADDITIVE::PARAMETERS;
}

}  // namespace

auto SOUND::ADDITIVE::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::ADDITIVE::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::ADDITIVE::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  return notation(index, held(instance, index));
}

auto SOUND::ADDITIVE::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return ::quantity(*static_cast<const Synth *>(instance), index);
}

auto SOUND::ADDITIVE::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  return ::sane(instance, index) && ADDITIVE::control(index, out);
}

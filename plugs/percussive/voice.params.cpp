// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto quantity(const PERCUSSIVE::Machine &machine, Whole index) -> Float {
  if (index == PERCUSSIVE::GAIN) return machine.gain;
  if (index == PERCUSSIVE::CHOKE) return machine.choke;
  const Whole slot = PERCUSSIVE::slot(index);
  if (slot >= PERCUSSIVE::SLOTS) return 0;
  return PERCUSSIVE::lane(index) == PERCUSSIVE::LEVEL ? machine.levels[slot]
                                                      : machine.decays[slot];
}

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < PERCUSSIVE::PARAMETERS;
}

}  // namespace

auto SOUND::PERCUSSIVE::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::PERCUSSIVE::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::PERCUSSIVE::SURFACE::reading(void *instance, Whole index)
  -> String {
  if (!::sane(instance, index)) return {};
  return notation(index, held(instance, index));
}

auto SOUND::PERCUSSIVE::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return ::quantity(*static_cast<const Machine *>(instance), index);
}

auto SOUND::PERCUSSIVE::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  return ::sane(instance, index) && PERCUSSIVE::control(index, out);
}

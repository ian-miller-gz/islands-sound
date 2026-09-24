// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < DELAY::PARAMETERS;
}

}  // namespace

auto SOUND::DELAY::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::DELAY::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::DELAY::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return static_cast<Trail *>(instance)->rows[index];
}

auto SOUND::DELAY::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  return notation(index, held(instance, index));
}

auto SOUND::DELAY::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  return ::sane(instance, index) && DELAY::control(index, out);
}

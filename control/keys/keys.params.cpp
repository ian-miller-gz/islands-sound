// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "keys.hpp"

namespace {

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index == SOUND::KEYS::OCTAVE;
}

}  // namespace

auto SOUND::KEYS::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::KEYS::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? "Octave" : String();
}

auto SOUND::KEYS::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return static_cast<const Board *>(instance)->octave;
}

auto SOUND::KEYS::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  char buffer[16];
  std::snprintf(
    buffer, sizeof(buffer), "%.0f", static_cast<double>(held(instance, index)));
  return buffer;
}

auto SOUND::KEYS::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  if (!::sane(instance, index)) return false;
  out = {.resting = RESTING, .least = LEAST, .most = MOST};
  return true;
}

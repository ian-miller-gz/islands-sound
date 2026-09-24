// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "transpose.hpp"

namespace {

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index == SOUND::TRANSPOSE::STEPS;
}

}  // namespace

auto SOUND::TRANSPOSE::clamped(Float value) -> Float {
  return value < LEAST ? LEAST : value > MOST ? MOST : value;
}

auto SOUND::TRANSPOSE::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::TRANSPOSE::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? "Steps" : String();
}

auto SOUND::TRANSPOSE::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return static_cast<const Shift *>(instance)->steps;
}

auto SOUND::TRANSPOSE::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  char buffer[16];
  std::snprintf(
    buffer, sizeof(buffer), "%+.0f",
    static_cast<double>(held(instance, index)));
  return buffer;
}

auto SOUND::TRANSPOSE::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  if (!::sane(instance, index)) return false;
  out = {.resting = RESTING, .least = LEAST, .most = MOST};
  return true;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "fader.hpp"

namespace {

auto least(Whole index) -> Float {
  return index == SOUND::FADER::PAN ? -SOUND::FADER::UNITY : 0.0f;
}

auto most(Whole index) -> Float {
  return index == SOUND::FADER::PAN ? SOUND::FADER::UNITY
                                    : SOUND::FADER::LOUDEST;
}

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < SOUND::FADER::PARAMETERS;
}

}  // namespace

auto SOUND::FADER::clamped(Whole index, Float value) -> Float {
  const Float floor = ::least(index);
  const Float ceiling = ::most(index);
  return value < floor ? floor : value > ceiling ? ceiling : value;
}

auto SOUND::FADER::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::FADER::SURFACE::name(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  return index == GAIN ? "Gain" : "Pan";
}

auto SOUND::FADER::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  const auto &level = *static_cast<const Level *>(instance);
  return index == GAIN ? level.gain : level.pan;
}

auto SOUND::FADER::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  char buffer[16];
  std::snprintf(
    buffer, sizeof(buffer), "%.2f", static_cast<double>(held(instance, index)));
  return buffer;
}

auto SOUND::FADER::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  if (!::sane(instance, index)) return false;
  out = {
    .resting = index == PAN ? CENTRE : UNITY,
    .least = ::least(index),
    .most = ::most(index)};
  return true;
}

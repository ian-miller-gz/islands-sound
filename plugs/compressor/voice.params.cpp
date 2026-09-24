// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < COMPRESSOR::PARAMETERS;
}

}  // namespace

auto SOUND::COMPRESSOR::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::COMPRESSOR::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::COMPRESSOR::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return static_cast<Press *>(instance)->rows[index];
}

auto SOUND::COMPRESSOR::SURFACE::reading(void *instance, Whole index)
  -> String {
  if (!::sane(instance, index)) return {};
  return notation(index, held(instance, index));
}

auto SOUND::COMPRESSOR::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  return ::sane(instance, index) && COMPRESSOR::control(index, out);
}

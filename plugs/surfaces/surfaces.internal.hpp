// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <island/audio.hpp>
#include <island/midi.hpp>

#include "surfaces.hpp"

namespace SOUND::SURFACES {

constexpr Float PEAK = 127.0f;
constexpr Float FULL = 32768.0f;
constexpr Whole ROOM = 1024;

struct Input {
  Whole rate = 0, channels = 0;
  AUDIO::Handle stream = AUDIO::INPUT::NONE;
  Whole lanes = 0;
  Whole lane = 0;
  Vector<AUDIO::Sample> scratch;
};

struct Output {
  Whole rate = 0, channels = 0;
  AUDIO::Handle stream = AUDIO::OUTPUT::NONE;
  Whole lanes = 0;
  Whole lane = 0;
  Vector<AUDIO::Sample> scratch;
};

struct Midiout {
  MIDI::Handle output = MIDI::OUTPUT::NONE;
};

void heard(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);
void sounded(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);
void spoken(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

auto hear(void *instance, const String &device, Whole lane) -> Flag;
auto sound(void *instance, const String &device, Whole lane) -> Flag;
auto speak(void *instance, const String &device, Whole lane) -> Flag;

}  // namespace SOUND::SURFACES

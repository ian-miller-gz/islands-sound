// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/audio.hpp>
#include <island/midi.hpp>

#include "surfaces.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::SURFACES;

auto taken(const String &device, Whole fallback) -> Whole {
  for (const AUDIO::INPUT::Device &row : AUDIO::INPUT::GET::devices())
    if (row.name == device && row.channels != 0)
      return std::min(row.channels, AUDIO::LANES);
  return fallback;
}

auto given(const String &device, Whole fallback) -> Whole {
  for (const AUDIO::OUTPUT::Device &row : AUDIO::OUTPUT::GET::devices())
    if (row.name == device && row.channels != 0)
      return std::min(row.channels, AUDIO::LANES);
  return fallback;
}

}  // namespace

auto SOUND::SURFACES::hear(void *instance, const String &device, Whole lane)
  -> Flag {
  if (instance == nullptr || device.empty()) return false;
  auto &input = *static_cast<Input *>(instance);
  input.lanes = ::taken(device, input.channels);
  input.lane = std::min(lane, input.lanes - 1);
  input.stream = AUDIO::INPUT::create(input.rate, input.lanes, device);
  if (input.stream == AUDIO::INPUT::NONE) return false;
  input.scratch.assign(::ROOM * input.lanes, 0);
  return true;
}
auto SOUND::SURFACES::sound(void *instance, const String &device, Whole lane)
  -> Flag {
  if (instance == nullptr || device.empty()) return false;
  auto &output = *static_cast<Output *>(instance);
  output.lanes = ::given(device, output.channels);
  output.lane = std::min(lane, output.lanes - 1);
  output.stream = AUDIO::OUTPUT::create(output.rate, output.lanes, device);
  if (output.stream == AUDIO::OUTPUT::NONE) return false;
  output.scratch.reserve(::ROOM * output.lanes);
  return true;
}
auto SOUND::SURFACES::speak(void *instance, const String &device, Whole)
  -> Flag {
  if (instance == nullptr || device.empty()) return false;
  auto &out = *static_cast<Midiout *>(instance);
  out.output = MIDI::OUTPUT::create(device);
  return out.output != MIDI::OUTPUT::NONE;
}

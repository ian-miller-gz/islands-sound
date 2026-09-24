// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

using namespace SOUND;

auto SOUND::CONTROL::inputs() -> Vector<AUDIO::INPUT::Device> {
  return AUDIO::INPUT::GET::devices();
}

auto SOUND::CONTROL::offered(const String &device) -> Flag {
  if (device.empty()) return true;
  for (const AUDIO::INPUT::Device &row : inputs())
    if (row.name == device) return true;
  return false;
}

void SOUND::CONTROL::choose(const String &device) {
  standing().input = {.device = device};
}

void SOUND::CONTROL::choose(Whole channel) {
  standing().input.channel = channel;
}

auto SOUND::CONTROL::chosen() -> Input { return standing().input; }

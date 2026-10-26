// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "control.hpp"

namespace SOUND::CONTROL {

struct Live {
  Whole track = NONE;
  String aim;
  Vector<AUDIO::PLUGIN::Event> arrived;
  Whole position = 0;
  Float tempo = TEMPO;
  Flag armed = false, taking = false, ready = false;
  Take take;
  Whole opened[PITCHES] = {};
  Flag holding[PITCHES] = {};
  Float force[PITCHES] = {};
  Whole stream = AUDIO::INPUT::NONE;
  Vector<AUDIO::Sample> scratch;
  Input input;
  Whole lanes = CHANNELS;
  Whole picked = 0;
};

auto standing() -> Live &;

auto values() -> Vector<Value> &;

auto same(const Address &one, const Address &two) -> Flag;

auto reached(const Live &live) -> Whole;

void capture(const AUDIO::PLUGIN::Event &edge);

void listening(Flag on);

}  // namespace SOUND::CONTROL

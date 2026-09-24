// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::PERCUSSIVE {

constexpr Whole SLOTS = 5;

constexpr Whole KICK = 0;
constexpr Whole SNARE = 1;
constexpr Whole CLOSED = 2;
constexpr Whole OPEN = 3;
constexpr Whole TOM = 4;

constexpr Whole LEVEL = 0;
constexpr Whole DECAY = 1;
constexpr Whole LANES = 2;
constexpr Whole GAIN = SLOTS * LANES;
constexpr Whole CHOKE = GAIN + 1;
constexpr Whole PARAMETERS = CHOKE + 1;

auto slot(Whole index) -> Whole;
auto lane(Whole index) -> Whole;

struct Recipe {
  STRING::Hot name;
  Whole pitch;
  Float hertz;
  Float floor;
  Float bend;
  Float tone;
  Float rasp;
  Float level;
  Float decay;
};
auto recipe(Whole slot) -> const Recipe &;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::PERCUSSIVE

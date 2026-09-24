// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::ADDITIVE {

constexpr Whole PARTIALS = 8;
constexpr Whole VOICES = 8;

constexpr Whole GAIN = 0;
constexpr Whole ATTACK = 1;
constexpr Whole DECAY = 2;
constexpr Whole SUSTAIN = 3;
constexpr Whole RELEASE = 4;
constexpr Whole PARTIAL = 5;
constexpr Whole PARAMETERS = PARTIAL + PARTIALS;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::ADDITIVE

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::DELAY {

constexpr Whole TIME = 0;
constexpr Whole FEEDBACK = 1;
constexpr Whole MIX = 2;
constexpr Whole PARAMETERS = 3;

constexpr Float BRIEFEST = 1.0f;
constexpr Float LONGEST = 1000.0f;
constexpr Float SPACED = 250.0f;

constexpr Float NOTHING = 0;
constexpr Float WHOLLY = 100.0f;
constexpr Float DEEPEST = 90.0f;
constexpr Float FEW = 35.0f;
constexpr Float SHADED = 25.0f;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::DELAY

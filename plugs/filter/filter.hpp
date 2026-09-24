// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::FILTER {

constexpr Whole RATE = 48000;

constexpr Whole MODE = 0;
constexpr Whole CUTOFF = 1;
constexpr Whole RESONANCE = 2;
constexpr Whole PARAMETERS = 3;

constexpr Whole LOW = 0;
constexpr Whole BAND = 1;
constexpr Whole HIGH = 2;
constexpr Whole MODES = 3;

constexpr Whole CLEAR = 3;
constexpr Float LOWEST = 20.0f;
constexpr Float HIGHEST = Float(RATE) / Float(CLEAR);

constexpr Float CALM = 0;
constexpr Float SHARPEST = 1.0f;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::FILTER

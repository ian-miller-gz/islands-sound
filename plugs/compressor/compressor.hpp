// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::COMPRESSOR {

constexpr Whole THRESHOLD = 0;
constexpr Whole RATIO = 1;
constexpr Whole ATTACK = 2;
constexpr Whole RELEASE = 3;
constexpr Whole MAKEUP = 4;
constexpr Whole PARAMETERS = 5;

constexpr Whole HALVED = 0;
constexpr Whole QUARTERED = 1;
constexpr Whole EIGHTHED = 2;
constexpr Whole LIMIT = 3;
constexpr Whole RATIOS = 4;

constexpr Float QUIETEST = -48.0f;
constexpr Float FULL = 0;
constexpr Float LOUDEST = 24.0f;

constexpr Float QUICKEST = 0.1f;
constexpr Float SLOWEST = 1000.0f;
constexpr Float BITING = 10.0f;
constexpr Float BREATHING = 100.0f;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::COMPRESSOR

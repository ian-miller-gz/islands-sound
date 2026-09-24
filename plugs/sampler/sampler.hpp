// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::SAMPLER {

constexpr Whole VOICES = 8;

constexpr Whole GAIN = 0;
constexpr Whole STOCK = 1;
constexpr Whole ROOT = 2;
constexpr Whole TUNE = 3;
constexpr Whole START = 4;
constexpr Whole LOOP = 5;
constexpr Whole ATTACK = 6;
constexpr Whole RELEASE = 7;
constexpr Whole PARAMETERS = 8;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::SAMPLER

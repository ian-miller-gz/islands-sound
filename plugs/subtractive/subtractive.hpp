// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../plug.hpp"

namespace SOUND::SUBTRACTIVE {

constexpr Whole VOICES = 8;
constexpr Whole OSCILLATORS = 2;

constexpr Whole SAW = 0;
constexpr Whole PULSE = 1;
constexpr Whole TRIANGLE = 2;

constexpr Whole GAIN = 0;
constexpr Whole SHAPE = 1;
constexpr Whole SPREAD = 2;
constexpr Whole CUTOFF = 3;
constexpr Whole RESONANCE = 4;
constexpr Whole DEPTH = 5;
constexpr Whole SWEEP = 6;
constexpr Whole CLOSE = 7;
constexpr Whole ATTACK = 8;
constexpr Whole DECAY = 9;
constexpr Whole SUSTAIN = 10;
constexpr Whole RELEASE = 11;
constexpr Whole PARAMETERS = 12;

auto control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
auto resting(Whole index) -> Float;
auto clamped(Whole index, Float value) -> Float;
auto label(Whole index) -> String;
auto notation(Whole index, Float value) -> String;

}  // namespace SOUND::SUBTRACTIVE

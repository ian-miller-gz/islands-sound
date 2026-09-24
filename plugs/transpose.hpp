// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../plug.hpp"

namespace SOUND::TRANSPOSE {

constexpr Whole STEPS = 0;
constexpr Whole PARAMETERS = 1;
constexpr Float LEAST = -24.0f;
constexpr Float MOST = 24.0f;
constexpr Float RESTING = 7.0f;

constexpr Whole HIGHEST = 127;

constexpr Whole SILENT = HIGHEST + 1;

struct Shift {
  Float steps = RESTING;
  Vector<Whole> sounding;
};

auto clamped(Float value) -> Float;

namespace SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SURFACE

}  // namespace SOUND::TRANSPOSE

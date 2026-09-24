// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../plug.hpp"

namespace SOUND::FADER {

constexpr Whole GAIN = 0;
constexpr Whole PAN = 1;
constexpr Whole PARAMETERS = 2;

constexpr Float UNITY = 1.0f;
constexpr Float CENTRE = 0.0f;
constexpr Float LOUDEST = 2.0f;

struct Level {
  Whole channels = 0;
  Float gain = UNITY;
  Float pan = CENTRE;
};

auto clamped(Whole index, Float value) -> Float;

namespace SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SURFACE

}  // namespace SOUND::FADER

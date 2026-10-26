// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"

namespace SOUND::TIMELINE::RATIO {

constexpr STRING::Hot WORD = "ratio";
constexpr Float PERCENT = 100.0f;
constexpr Float LEAST = 25.0f;
constexpr Float MOST = 400.0f;

using Clip = ARRANGEMENT::TRACK::LANE::Clip;

auto held(Float percent) -> Float;

auto covered(const Clip &clip) -> Whole;

auto stretched(const Clip &clip, Float percent) -> Clip;

auto drawn(const Clip &clip, Whole frames) -> Clip;

}  // namespace SOUND::TIMELINE::RATIO

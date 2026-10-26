// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "stretch.hpp"

namespace SOUND::STRETCH {

constexpr Float SAME = 1.0f;

auto length(const Take &take) -> Whole;

auto summed(const Take &take) -> Vector<Float>;

auto scaled(const Take &take, Float scale) -> Take;

auto resampled(const Take &take, Whole frames) -> Take;

}  // namespace SOUND::STRETCH

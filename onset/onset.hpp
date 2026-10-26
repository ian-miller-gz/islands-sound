// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"
#include "../stock.hpp"

namespace SOUND::ONSET {

constexpr Whole HOP = 256;
constexpr Whole GAP = RATE / 20;
constexpr Float FLOOR = 0.05f;
constexpr Float RESTING = 0.5f;

auto found(const Take &take, Float sense) -> Vector<Whole>;

}  // namespace SOUND::ONSET

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"
#include "../stock.hpp"

namespace SOUND::CONTOUR {

constexpr Whole HOP = 512;
constexpr Whole WINDOW = 2048;
constexpr Whole DOWN = 4;
constexpr Float LOWEST = 50.0f, HIGHEST = 1000.0f;
constexpr Float DIP = 0.15f;
constexpr Float QUIET = 0.001f;
constexpr Float STEADY = 50.0f;
constexpr Whole SHORTEST = 4;

struct Found {
  Whole at = 0, frames = 0, pitch = 0;
  Integer cents = 0;
};

auto traced(const Take &take) -> Vector<Float>;

auto noted(const Vector<Float> &trace) -> Vector<Found>;

}  // namespace SOUND::CONTOUR

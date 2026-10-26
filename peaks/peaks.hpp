// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../stock.hpp"

namespace SOUND::PEAKS {

constexpr Whole GRAIN = 256;

struct Peak {
  Float least = 0.0f, most = 0.0f;
};

struct Level {
  Vector<Peak> grains;
};

auto level(const Take &take) -> Level;

auto peak(const Take &take, const Level &level, Whole from, Whole frames)
  -> Peak;

namespace GET {

auto level(Whole stock) -> const Level &;

}  // namespace GET

}  // namespace SOUND::PEAKS

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND::VIEWS::ROLL {

constexpr Whole TONAL = 0;
constexpr Whole PERCUSSION = 1;

namespace PRESENTATION {
constexpr Whole GRID = 0, STAFF = 1;
}  // namespace PRESENTATION

namespace GET {

auto notation() -> Whole;

auto presentation() -> Whole;

auto staffed() -> Flag;
auto pitched() -> Flag;

}  // namespace GET

}  // namespace SOUND::VIEWS::ROLL

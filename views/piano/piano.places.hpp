// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <limits>

#include <island/gui/gui.hpp>

namespace SOUND::VIEWS::ROLL::STAFF {

constexpr Integer TOP = -6, FOOT = 13, STRAYS = 14;
constexpr Integer NOWHERE = std::numeric_limits<Integer>::min();
constexpr Integer LINES[] = {0, 2, 4, 6, 8};
constexpr Integer GRAND[] = {0, 2, 4, 6, 8, 12, 14, 16, 18, 20};
constexpr Integer REFERENCE = 38;
constexpr Integer TREBLE_G = 6, BASS_F = 14;
constexpr Float SCALE = 1.0f;

auto placed(Whole pitch) -> Integer;

auto stepped(Whole pitch, Integer by) -> Whole;

auto top() -> Integer;

auto lane(Whole pitch, Whole at) -> Float;

auto pitch(Float y, Whole at) -> Whole;

auto deep() -> Float;

auto spanned() -> GUI::Extent;

}  // namespace SOUND::VIEWS::ROLL::STAFF

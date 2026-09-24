// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include <island/graphics/sprites/sprites.hpp>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BOUND = "icons";
constexpr Whole SIDE = 16;
constexpr char LIT = '#';
constexpr Byte SHOWN = 255;
constexpr Byte CLEAR = 0;

constexpr STRING::Hot MAPPING[] = {
  "................", "......####......", "......####......",
  "......####......", "......####......", ".....##..##.....",
  "....##....##....", "....##....##....", "...##......##...",
  "...##......##...", "..##........##..", ".####......####.",
  ".####......####.", ".####......####.", ".####......####.",
  "................"};

constexpr STRING::Hot const *MARKS[] = {::MAPPING};

constexpr auto squared(STRING::Hot const *mark) -> Flag {
  for (Whole row = 0; row < ::SIDE; ++row) {
    Whole wide = 0;
    while (mark[row][wide] != '\0') ++wide;
    if (wide != ::SIDE) return false;
  }
  return true;
}
static_assert(std::size(::MAPPING) == ::SIDE && ::squared(::MAPPING));

auto covered(STRING::Hot const *mark) -> GFX::SPRITES::ATLASES::Mask {
  GFX::SPRITES::ATLASES::Mask cell;
  cell.coverage.reserve(::SIDE * ::SIDE);
  for (Whole row = 0; row < ::SIDE; ++row)
    for (Whole column = 0; column < ::SIDE; ++column)
      cell.coverage.push_back(mark[row][column] == ::LIT ? ::SHOWN : ::CLEAR);
  return cell;
}

}  // namespace

void SOUND::VIEWS::ICONS::create() {
  Vector<GFX::SPRITES::ATLASES::Mask> cells;
  for (STRING::Hot const *mark : ::MARKS) cells.push_back(::covered(mark));
  GUI::bind(::BOUND, GFX::SPRITES::ATLASES::create({::SIDE, ::SIDE}, cells));
}

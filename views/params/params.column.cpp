// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/tip.hpp>

#include "../views.letters.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;
namespace RACK = SOUND::VIEWS::RACK;

constexpr STRING::Hot ROWS = "params.rows";
constexpr STRING::Hot PATTERN = "rig";
constexpr Float LETTER = 15.0f;
constexpr Float AIR = 10.0f;
constexpr Float SHARE = 0.5f;

auto id(const String &cell, STRING::Hot part) -> String {
  return cell + "." + part;
}

auto at(GUI::Handle page, STRING::Hot part) -> GUI::Position {
  return GUI::GET::position(page, ::id(::PATTERN, part).c_str());
}

auto wide(GUI::Handle page, Float column) -> Float {
  return column - ::at(page, RACK::NAMED).x - ::AIR;
}

}  // namespace

auto SOUND::VIEWS::RACK::column(GUI::Handle page, const Vector<Row> &wanted)
  -> Float {
  Whole widest = 0;
  for (const Row &row : wanted)
    if (!head(row)) widest = std::max(widest, UNICODE::columns(titled(row)));
  const Float run =
    Float(widest) * LETTERS::ADVANCE * ::LETTER + ::at(page, NAMED).x + ::AIR;
  return std::min(run, ::SHARE * GUI::GET::measured(page, ::ROWS).w);
}

auto SOUND::VIEWS::RACK::cut(GUI::Handle page, const Row &row, Float column)
  -> String {
  return LETTERS::cropped(titled(row), ::wide(page, column), ::LETTER);
}

void SOUND::VIEWS::RACK::seat(
  GUI::Handle page, const String &cell, Float column) {
  const GUI::Position door = ::at(page, STEPPED);
  const GUI::Position bar = ::at(page, BARRED);
  const Float tall = GUI::GET::extent(page, ::id(::PATTERN, NAMED).c_str()).h;
  GUI::set(
    page, ::id(cell, NAMED).c_str(), GUI::Extent{::wide(page, column), tall});
  GUI::set(page, ::id(cell, STEPPED).c_str(), GUI::Position{column, door.y});
  GUI::set(
    page, ::id(cell, BARRED).c_str(),
    GUI::Position{column + bar.x - door.x, bar.y});
}

void SOUND::VIEWS::RACK::title(
  GUI::Handle page, const String &cell, const Row &row, Float column) {
  const String id = ::id(cell, NAMED);
  const String whole = titled(row);
  const String shown = cut(page, row, column);
  GUI::set(page, id.c_str(), GUI::Text{shown});
  GUI::TIP::note(page, id.c_str(), shown == whole ? String() : whole);
}

void SOUND::VIEWS::RACK::show(
  GUI::Handle page, const String &cell, const Row &row, Float column) {
  const Flag top = head(row);
  GUI::set(page, ::id(cell, HEADED).c_str(), GUI::Visibility{top});
  for (STRING::Hot part : {NAMED, BARRED})
    GUI::set(page, ::id(cell, part).c_str(), GUI::Visibility{!top});
  if (top) {
    for (STRING::Hot part : {STEPPED, MARKED})
      GUI::set(page, ::id(cell, part).c_str(), GUI::Visibility{false});
    GUI::set(page, ::id(cell, HEADED).c_str(), GUI::Text{headed(row)});
    return;
  }
  seat(page, cell, column);
  title(page, cell, row, column);
}

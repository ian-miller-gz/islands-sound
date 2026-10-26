// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/tip.hpp>

#include "../../control.hpp"
#include "../views.letters.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;
namespace RACK = SOUND::VIEWS::RACK;

constexpr STRING::Hot ROWS = "params.rows";

auto id(const String &cell, STRING::Hot part) -> String {
  return cell + "." + part;
}

auto at(GUI::Handle page, STRING::Hot part) -> GUI::Position {
  return GUI::GET::position(page, ::id(VIEWS::RIG::PATTERN, part).c_str());
}

auto wide(GUI::Handle page, Float column) -> Float {
  return column - ::at(page, RACK::NAMED).x - VIEWS::RIG::AIR;
}

auto part(GUI::Handle page, STRING::Hot part) -> GUI::Extent {
  return GUI::GET::extent(page, ::id(VIEWS::RIG::PATTERN, part).c_str());
}

auto room(GUI::Handle page, const String &cell, const RACK::Row &row) -> Float {
  const Flag engaged = RACK::listening(row) ||
                       CONTROL::MAP::bound(RACK::addressed(row)).number != NONE;
  const Float mark = ::part(page, RACK::BOUND).w + VIEWS::RIG::AIR;
  return GUI::GET::measured(page, cell.c_str()).w - (engaged ? mark : 0.0f);
}

auto door(GUI::Handle page, const RACK::Row &row) -> Float {
  if (!CONTROL::overridden(RACK::addressed(row))) return 0.0f;
  return ::part(page, VIEWS::RIG::RESTORED).w + VIEWS::RIG::AIR;
}

}  // namespace

auto SOUND::VIEWS::RACK::column(GUI::Handle page, const Vector<Row> &wanted)
  -> Float {
  Vector<String> names;
  for (const Row &row : wanted)
    if (!head(row)) names.push_back(titled(row));
  return RIG::column(page, ::ROWS, names);
}

auto SOUND::VIEWS::RACK::cut(GUI::Handle page, const Row &row, Float column)
  -> String {
  return LETTERS::cropped(titled(row), ::wide(page, column), RIG::LETTER);
}

void SOUND::VIEWS::RACK::seat(
  GUI::Handle page, const String &cell, const Row &row, Float column) {
  const Float door = ::door(page, row);
  RIG::seat(page, cell, column, ::room(page, cell, row) - door, door);
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
  GUI::set(page, ::id(cell, RIG::HINTED).c_str(), GUI::Visibility{false});
  for (STRING::Hot part : {NAMED, DIALLED})
    GUI::set(page, ::id(cell, part).c_str(), GUI::Visibility{!top});
  if (top) {
    for (STRING::Hot part : {STEPPED, MARKED, RIG::RESTORED})
      GUI::set(page, ::id(cell, part).c_str(), GUI::Visibility{false});
    GUI::set(page, ::id(cell, HEADED).c_str(), GUI::Text{headed(row)});
    return;
  }
  seat(page, cell, row, column);
  title(page, cell, row, column);
}

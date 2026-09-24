// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

Whole standing = 0;

struct Stood {
  GUI::NGA::Zoom scale;
  GUI::NGA::Pan pan;
  Flag seen = false;
};
Vector<Stood> stood;

auto boarded(const VIEWS::View &unit) -> Flag {
  return VIEWS::owns(String(unit.name), VIEWS::ZOOM) ||
         VIEWS::owns(String(unit.name), VIEWS::PAN);
}

auto board(const VIEWS::View &unit) -> String {
  return String(unit.name) + "." + VIEWS::BOARD;
}

struct Place {
  GUI::Position at;
  Flag seen = false;
};
Vector<Place> places;

struct Mark {
  Whole rows = 0, cursor = 0;
  Flag seen = false;
};
Vector<Mark> marks;

auto listed(const VIEWS::View &unit) -> String {
  return String(unit.name) + "." + VIEWS::ROWS;
}

auto grips() -> Vector<String> {
  Vector<String> worn;
  worn.reserve(VIEWS::roster().size() + 1);
  for (const VIEWS::View &unit : VIEWS::roster())
    worn.push_back(String(unit.name) + "." + VIEWS::HANDLED);
  worn.emplace_back(VIEWS::BAR);
  return worn;
}

auto authored(const String &id) -> Flag {
  return GUI::GET::extent(VIEWS::sheet(), id.c_str()).h > 0.0f;
}

void recorded() {
  const Vector<VIEWS::View> &roster = VIEWS::roster();
  ::stood.resize(roster.size());
  for (Whole row = 0; row < roster.size(); ++row) {
    if (!::boarded(roster[row])) continue;
    const String id = ::board(roster[row]);
    const GUI::NGA::Zoom scale =
      GUI::NGA::GET::zoom(VIEWS::sheet(), id.c_str());
    if (scale.value <= 0.0f) continue;
    ::stood[row] = {
      scale, GUI::NGA::GET::pan(VIEWS::sheet(), id.c_str()), true};
  }
  const Vector<String> worn = ::grips();
  ::places.resize(worn.size());
  for (Whole row = 0; row < worn.size(); ++row)
    if (::authored(worn[row]))
      ::places[row] = {
        GUI::GET::position(VIEWS::sheet(), worn[row].c_str()), true};
  ::marks.resize(roster.size());
  for (Whole row = 0; row < roster.size(); ++row) {
    const String id = ::listed(roster[row]);
    const Whole rows = GUI::GET::rows(VIEWS::sheet(), id.c_str());
    if (rows == 0) continue;
    ::marks[row] = {rows, GUI::GET::cursor(VIEWS::sheet(), id.c_str()), true};
  }
}

void restored() {
  const Vector<VIEWS::View> &roster = VIEWS::roster();
  for (Whole row = 0; row < roster.size() && row < ::stood.size(); ++row) {
    if (!::stood[row].seen) continue;
    const String id = ::board(roster[row]);
    GUI::NGA::set(VIEWS::sheet(), id.c_str(), ::stood[row].scale);
    GUI::NGA::set(VIEWS::sheet(), id.c_str(), ::stood[row].pan);
  }
  const Vector<String> worn = ::grips();
  for (Whole row = 0; row < worn.size() && row < ::places.size(); ++row)
    if (::places[row].seen)
      GUI::set(VIEWS::sheet(), worn[row].c_str(), ::places[row].at);
  for (Whole row = 0; row < roster.size() && row < ::marks.size(); ++row) {
    if (!::marks[row].seen) continue;
    const String id = ::listed(roster[row]);
    GUI::set(VIEWS::sheet(), id.c_str(), GUI::Rows{::marks[row].rows});
    GUI::set(VIEWS::sheet(), id.c_str(), GUI::Cursor{::marks[row].cursor});
  }
}

}  // namespace

void SOUND::VIEWS::rebirth() {
  const Whole count = GUI::GET::generation(sheet());
  if (count != ::standing && ::standing != 0) ::restored();
  ::standing = count;
  ::recorded();
}

auto SOUND::VIEWS::reborn(Whole &born) -> Flag {
  if (born == ::standing) return false;
  born = ::standing;
  return true;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot VIEWED = "view";
constexpr STRING::Hot X = "x";
constexpr STRING::Hot Y = "y";
constexpr STRING::Hot MARKED = "cursor";

auto boarded(const VIEWS::View &unit) -> Flag {
  return VIEWS::owns(String(unit.name), VIEWS::ZOOM) ||
         VIEWS::owns(String(unit.name), VIEWS::PAN);
}

auto grips() -> Vector<String> {
  Vector<String> worn;
  for (const VIEWS::View &unit : VIEWS::roster())
    worn.push_back(String(unit.name) + "." + VIEWS::HANDLED);
  worn.emplace_back(VIEWS::BAR);
  return worn;
}

void seated(const FIELDS::Map &face, const String &grip) {
  const GUI::Handle page = VIEWS::sheet();
  GUI::Position at = GUI::GET::position(page, grip.c_str());
  const Flag across = VIEWS::KEPT::number(face, grip + "." + ::X, at.x);
  const Flag down = VIEWS::KEPT::number(face, grip + "." + ::Y, at.y);
  if (across || down) GUI::set(page, grip.c_str(), at);
}

}  // namespace

auto SOUND::VIEWS::kept() -> FIELDS::Map {
  FIELDS::Map face;
  face[::VIEWED] = String(standing());
  for (const View &unit : roster()) {
    if (::boarded(unit)) VIEWS::KEPT::board(face, unit);
    const String rows = String(unit.name) + "." + ROWS;
    if (GUI::GET::rows(sheet(), rows.c_str()) != 0)
      face[rows + "." + ::MARKED] = GUI::GET::cursor(sheet(), rows.c_str());
    if (unit.keep != nullptr) unit.keep(face);
  }
  for (const String &grip : ::grips()) {
    const GUI::Position at = GUI::GET::position(sheet(), grip.c_str());
    face[grip + "." + ::X] = at.x;
    face[grip + "." + ::Y] = at.y;
  }
  return face;
}

void SOUND::VIEWS::keep(const FIELDS::Map &face) {
  const auto viewed = face.find(::VIEWED);
  if (viewed != face.end())
    if (const String *name = std::get_if<String>(&viewed->second))
      choose(*name);
  for (const View &unit : roster()) {
    if (::boarded(unit)) VIEWS::KEPT::boarded(face, unit);
    const String rows = String(unit.name) + "." + ROWS;
    const auto marked = face.find(rows + "." + ::MARKED);
    if (marked != face.end())
      if (const Whole *cursor = std::get_if<Whole>(&marked->second))
        GUI::set(sheet(), rows.c_str(), GUI::Cursor{*cursor});
    if (unit.restore != nullptr) unit.restore(face);
  }
  for (const String &grip : ::grips()) ::seated(face, grip);
}

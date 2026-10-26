// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

struct Place {
  STRING::Hot id = "";
  Whole region = NONE;
};

constexpr Place PLACES[] = {
  {VIEWS::SPLIT::GROUND::LOWER, BELOW},
  {VIEWS::SPLIT::BAND, BELOW},
  {VIEWS::SPLIT::GROUND::UPPER, UPPER}};

auto inside(Float at, Float from, Float span) -> Flag {
  return at >= from && at < from + span;
}

auto boards() -> Vector<String> {
  Vector<String> ids;
  for (const Whole at : {VIEWS::chosen(), VIEWS::SPLIT::lower()})
    ids.push_back(VIEWS::GET::board(VIEWS::roster()[at]));
  for (const String &head : VIEWS::SPLIT::heads())
    for (const STRING::Hot tail : {VIEWS::BOARD, VIEWS::MAP})
      ids.push_back(head + VIEWS::SPLIT::DOT + tail);
  return ids;
}

auto reached(GUI::Handle page, const String &board) -> Flag {
  const STRING::Hot id = board.c_str();
  return !GUI::NGA::GET::stroked(page, id).board.empty() ||
         !GUI::NGA::GET::pressed(page, id).board.empty();
}

}  // namespace

auto SOUND::VIEWS::SPLIT::landed(GUI::Handle page) -> String {
  const String over = GUI::GET::hover(page);
  if (!over.empty()) return over;
  for (const String &board : ::boards())
    if (::reached(page, board)) return board;
  return {};
}

auto SOUND::VIEWS::SPLIT::placed(const String &id) -> Whole {
  for (const ::Place &place : ::PLACES)
    if (id == place.id) return place.region;
  for (const View &unit : roster()) {
    const String icon = String(BAR) + DOT + unit.name;
    if (!unit.panel && (id == icon || id.starts_with(icon + DOT))) return BELOW;
  }
  return NONE;
}

auto SOUND::VIEWS::SPLIT::spotted(GUI::Handle page) -> Whole {
  const GUI::Position at = spot();
  const GUI::Position corner = GUI::GET::origin(page, MAIN);
  const GUI::Extent size = GUI::GET::measured(page, MAIN);
  if (!::inside(at.x, corner.x, size.w) || !::inside(at.y, corner.y, size.h))
    return NONE;
  const Float top = GUI::GET::position(page, BAR).y;
  if (at.y < top) return UPPER;
  if (at.y >= top + GUI::GET::extent(page, BAR).h) return BELOW;
  return NONE;
}

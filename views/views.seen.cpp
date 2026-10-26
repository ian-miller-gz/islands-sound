// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.seen.hpp"
#include "views.split.hpp"

auto SOUND::VIEWS::GET::seen(const View &unit) -> Seen {
  const String id = board(unit);
  return {
    face(unit).name, GUI::NGA::GET::pan(VIEWS::sheet(), id.c_str()),
    GUI::NGA::GET::zoom(VIEWS::sheet(), id.c_str())};
}

auto SOUND::VIEWS::GET::seen() -> Seen {
  const View *unit = SPLIT::aimed();
  if (unit == nullptr || !owns(unit->name, ZOOM)) return {};
  const SPLIT::Turn turn(SPLIT::turned());
  return seen(*unit);
}

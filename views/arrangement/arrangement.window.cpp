// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "arrangement.internal.hpp"

namespace {

constexpr Float STILL = 0.0f;

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::seen() -> Seen {
  const Float wide = window();
  if (wide <= ::STILL) return {};
  const Float west = GUI::NGA::GET::pan(document(), board().c_str()).x;
  const Float from = west > ::STILL ? west : ::STILL;
  return {Whole(from * Float(GRAIN)), Whole((from + wide) * Float(GRAIN))};
}

auto SOUND::VIEWS::ARRANGEMENT::window() -> Float {
  const GUI::Handle page = document();
  const String board = VIEWS::ARRANGEMENT::board();
  return GUI::SAC::WINDOW::seen(
           GUI::GET::measured(page, board.c_str()),
           GUI::NGA::GET::zoom(page, board.c_str()))
    .w;
}

auto SOUND::VIEWS::ARRANGEMENT::least() -> GUI::Extent {
  const GUI::Extent seen = GUI::GET::measured(document(), board().c_str());
  const Float floor = GUI::SAC::WINDOW::least(seen.w, across(TEN_HOURS));
  return {floor, floor};
}

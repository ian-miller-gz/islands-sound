// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "arrangement.internal.hpp"

void SOUND::VIEWS::ARRANGEMENT::chase() {
  const GUI::Handle page = document();
  const String board = VIEWS::ARRANGEMENT::board();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, board.c_str());
  GUI::NGA::set(
    page, board.c_str(),
    GUI::NGA::Pan{
      VIEWS::chased(held.x, across(VIEWS::clock()), window()), held.y});
}

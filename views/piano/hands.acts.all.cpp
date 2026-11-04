// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/carry.hpp>
#include <island/gui/seat.hpp>

#include "piano.acts.hpp"
#include "piano.internal.hpp"

void SOUND::VIEWS::ROLL::all(GUI::Handle page) {
  const String board = VIEWS::ROLL::board();
  const Vector<Pip> &shown = plates();
  Vector<String> marks;
  for (Whole row = 0; row < shown.size(); ++row)
    if (shown[row].stock != NONE && shown[row].index != NONE)
      marks.push_back(GUI::SAC::SEAT::named(board.c_str(), row));
  GUI::SAC::CARRY::follow(page, marks);
}

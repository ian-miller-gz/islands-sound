// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;

Whole answered = 0;

auto noted(const Vector<Noted> &notes, const VIEWS::ROLL::Pip &pip) -> Flag {
  for (const Noted &one : notes)
    if (one.stock == pip.stock && one.index == pip.index) return true;
  return false;
}

}  // namespace

void SOUND::VIEWS::ROLL::walk(GUI::Handle page) {
  const Walk &walked = HISTORY::walked();
  if (walked.mark == ::answered) return;
  ::answered = walked.mark;
  if (!walked.writing) return;
  const String board = ROLL::board();
  const Vector<Pip> &drawn = plates();
  for (Whole row = 0; row < drawn.size(); ++row)
    GUI::NGA::set(
      page, GUI::SAC::SEAT::named(board.c_str(), row).c_str(),
      GUI::NGA::Selected{::noted(walked.notes, drawn[row])});
}

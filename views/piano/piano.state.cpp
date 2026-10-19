// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;

auto marked() -> String {
  const Whole row =
    GUI::SAC::SEAT::selected(VIEWS::document(), VIEWS::ROLL::board().c_str());
  return row == NONE ? String("none") : std::to_string(row);
}

auto noted() -> Whole {
  Whole count = 0;
  for (const VIEWS::ROLL::Pip &pip : VIEWS::ROLL::plates())
    if (pip.stock != NONE) ++count;
  return count;
}

void notes(SHELL::Session &session) {
  const Vector<VIEWS::ROLL::Pip> &shown = VIEWS::ROLL::plates();
  for (Whole row = 0; row < shown.size(); ++row)
    if (shown[row].stock == NONE)
      session.print(std::format(
        "loose {} pitch {} at {} length {}", row, shown[row].pitch,
        shown[row].at, shown[row].length));
    else
      session.print(std::format(
        "note {} stock {} pitch {} at {} length {}", row, shown[row].stock,
        shown[row].pitch, shown[row].at, shown[row].length));
}

}  // namespace

void SOUND::VIEWS::ROLL::state(SHELL::Session &session) {
  session.print(std::format(
    "piano notes {} grain {} chosen {} pan {:g} zoom {:g}", ::noted(), GRAIN,
    ::marked(), GUI::NGA::GET::pan(document(), board().c_str()).x, scaled()));
  const GUI::NGA::Zoom scale = GUI::NGA::GET::zoom(document(), board().c_str());
  session.print(
    std::format("scale across {:g} down {:g}", scale.value, scale.down));
  dials(session);
  doors(session);
  easel(session);
  keys(session);
  rail(session);
  dressed(session);
  grid(session);
  head(session);
  ruler(session);
  cycle(session);
  range(session);
  seams(session);
  ::notes(session);
}

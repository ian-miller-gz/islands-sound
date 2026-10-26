// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

constexpr Float STILL = 0.0f;

constexpr Float UNTURNED = 0.0f;

constexpr Float STEP = 1.1f;

auto lowered() -> const VIEWS::View * {
  const Whole row = VIEWS::SPLIT::lower();
  const Flag own = row < VIEWS::roster().size() && row != VIEWS::chosen();
  return own ? &VIEWS::roster()[row] : nullptr;
}

}  // namespace

void SOUND::VIEWS::wheeled() {
  for (const View *unit : {found(String(standing())), ::lowered()})
    if (unit != nullptr && owns(unit->name, ZOOM))
      wheeled(*unit, GUI::NGA::GET::wheel(sheet(), GET::board(*unit).c_str()));
}

void SOUND::VIEWS::wheeled(const GUI::NGA::Wheel &turn) {
  const View *unit = found(String(standing()));
  if (unit != nullptr) wheeled(*unit, turn);
}

void SOUND::VIEWS::wheeled(const View &unit, const GUI::NGA::Wheel &turn) {
  if (!owns(unit.name, ZOOM)) return;
  const View &face = GET::face(unit);
  const String board = GET::board(unit);
  if (turn.turns == ::UNTURNED || (!face.split && !turn.control)) return;
  const GUI::NGA::Zoom scale = GUI::NGA::GET::zoom(sheet(), board.c_str());
  if (scale.value <= ::STILL || scale.down <= ::STILL) return;
  const Float by = std::pow(::STEP, turn.turns);
  const Flag deep = face.split && turn.control;
  const Float across = deep ? scale.value : scale.value * by;
  const Float down = face.split && !deep ? scale.down : scale.down * by;
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(sheet(), board.c_str());
  if (!zoom(unit, across, down)) return;
  const GUI::NGA::Zoom now = GUI::NGA::GET::zoom(sheet(), board.c_str());
  pan(
    unit, GUI::SAC::WINDOW::about(held.x, turn.across, scale.value, now.value),
    face.flat
      ? held.y
      : GUI::SAC::WINDOW::about(held.y, turn.down, scale.down, now.down));
}

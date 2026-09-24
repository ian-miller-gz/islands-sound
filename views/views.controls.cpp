// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "../boards.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float STILL = 0.0f;

constexpr Float UNTURNED = 0.0f;

constexpr Float STEP = 1.1f;

constexpr Float FLAT = 1.0f;

auto stood(const VIEWS::View &unit) -> const VIEWS::View & {
  return unit.faced == nullptr ? unit : ::stood(*unit.faced());
}

auto board(const VIEWS::View &unit) -> String {
  return String(::stood(unit).name) + "." + VIEWS::BOARD;
}

auto floors(const VIEWS::View &face) -> GUI::Extent {
  return face.least == nullptr ? GUI::Extent{} : face.least();
}

auto ceilings(const VIEWS::View &face) -> GUI::Extent {
  return face.most == nullptr ? GUI::Extent{} : face.most();
}

auto clamped(Float factor, Float floor, Float ceiling) -> Float {
  const Float held = floor > ::STILL ? std::max(factor, floor) : factor;
  return ceiling > ::STILL ? std::min(held, ceiling) : held;
}

auto settled(const VIEWS::View &unit, Float across, Float down)
  -> GUI::NGA::Zoom {
  const VIEWS::View &face = ::stood(unit);
  const GUI::Extent floor = ::floors(face), ceiling = ::ceilings(face);
  if (face.flat) return {::clamped(across, floor.w, ceiling.w), ::FLAT};
  if (face.split)
    return {
      ::clamped(across, floor.w, ceiling.w),
      ::clamped(down, floor.h, ceiling.h)};
  return {::clamped(across, floor.w, ceiling.w), GUI::NGA::Zoom::SAME};
}

}  // namespace

auto SOUND::VIEWS::zoom(Float factor) -> Flag { return zoom(factor, factor); }

auto SOUND::VIEWS::zoom(Float across, Float down) -> Flag {
  const View *unit = found(String(standing()));
  if (
    across <= ::STILL || down <= ::STILL || unit == nullptr ||
    !owns(standing(), ZOOM))
    return false;
  return GUI::NGA::set(
           sheet(), ::board(*unit).c_str(), ::settled(*unit, across, down)) ==
         0;
}

auto SOUND::VIEWS::pan(Float x, Float y) -> Flag {
  const View *unit = found(String(standing()));
  if (unit == nullptr || !owns(standing(), PAN)) return false;
  return GUI::NGA::set(sheet(), ::board(*unit).c_str(), GUI::NGA::Pan{x, y}) ==
         0;
}

void SOUND::VIEWS::wheeled() {
  const View *unit = found(String(standing()));
  if (unit == nullptr || !owns(standing(), ZOOM)) return;
  wheeled(GUI::NGA::GET::wheel(sheet(), ::board(*unit).c_str()));
}

void SOUND::VIEWS::wheeled(const GUI::NGA::Wheel &turn) {
  const View *unit = found(String(standing()));
  if (unit == nullptr || !owns(standing(), ZOOM)) return;
  const View &face = ::stood(*unit);
  const String board = ::board(*unit);
  if (turn.turns == ::UNTURNED || (!face.split && !turn.control)) return;
  const GUI::NGA::Zoom scale = GUI::NGA::GET::zoom(sheet(), board.c_str());
  if (scale.value <= ::STILL || scale.down <= ::STILL) return;
  const Float by = std::pow(::STEP, turn.turns);
  const Flag deep = face.split && turn.control;
  const Float across = deep ? scale.value : scale.value * by;
  const Float down = face.split && !deep ? scale.down : scale.down * by;
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(sheet(), board.c_str());
  if (!zoom(across, down)) return;
  const GUI::NGA::Zoom now = GUI::NGA::GET::zoom(sheet(), board.c_str());
  pan(
    GUI::SAC::WINDOW::about(held.x, turn.across, scale.value, now.value),
    face.flat
      ? held.y
      : GUI::SAC::WINDOW::about(held.y, turn.down, scale.down, now.down));
}

auto SOUND::VIEWS::cursor(const String &name) -> Whole {
  return BOARDS::cursor(sheet(), (name + "." + ROWS).c_str());
}

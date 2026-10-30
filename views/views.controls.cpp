// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "../boards.hpp"
#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

constexpr Float STILL = 0.0f;

constexpr Float FLAT = 1.0f;

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
  const VIEWS::View &face = VIEWS::GET::face(unit);
  const GUI::Extent floor = ::floors(face), ceiling = ::ceilings(face);
  if (face.flat) return {::clamped(across, floor.w, ceiling.w), ::FLAT};
  if (face.split)
    return {
      ::clamped(across, floor.w, ceiling.w),
      ::clamped(down, floor.h, ceiling.h)};
  return {::clamped(across, floor.w, ceiling.w), GUI::NGA::Zoom::SAME};
}

}  // namespace

auto SOUND::VIEWS::GET::face(const View &unit) -> const View & {
  return unit.faced == nullptr ? unit : face(*unit.faced());
}

auto SOUND::VIEWS::GET::board(const View &unit) -> String {
  return String(face(unit).name) + "." + BOARD;
}

auto SOUND::VIEWS::zoom(Float factor) -> Flag { return zoom(factor, factor); }

auto SOUND::VIEWS::zoom(Float across, Float down) -> Flag {
  const View *unit = SPLIT::aimed();
  return unit != nullptr && zoom(*unit, across, down);
}

auto SOUND::VIEWS::zoom(const View &unit, Float across, Float down) -> Flag {
  if (across <= ::STILL || down <= ::STILL || !owns(unit.name, ZOOM))
    return false;
  return GUI::NGA::set(
           sheet(), GET::board(unit).c_str(), ::settled(unit, across, down)) ==
         0;
}

auto SOUND::VIEWS::pan(Float x, Float y) -> Flag {
  const View *unit = SPLIT::aimed();
  return unit != nullptr && pan(*unit, x, y);
}

auto SOUND::VIEWS::pan(const View &unit, Float x, Float y) -> Flag {
  if (!owns(unit.name, PAN)) return false;
  return GUI::NGA::set(
           sheet(), GET::board(unit).c_str(), GUI::NGA::Pan{x, y}) == 0;
}

auto SOUND::VIEWS::cursor(const String &name) -> Whole {
  return BOARDS::cursor(sheet(), (name + "." + ROWS).c_str());
}

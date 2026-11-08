// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::SHUT;

void placed(GUI::Handle page, STRING::Hot root, Float down, Float margin) {
  GUI::Position at = GUI::GET::position(page, root);
  GUI::Extent size = GUI::GET::extent(page, root);
  if (at.y != down) GUI::set(page, root, GUI::Position{at.x, down});
  if (size.h != margin) GUI::set(page, root, GUI::Extent{size.w, margin});
}

auto above(const VIEWS::SPLIT::Seat &seat, Float tall) -> Float {
  return seat.depth - tall + seat.bar + seat.least;
}

constexpr STRING::Hot HANDLE = ".handle";
constexpr STRING::Hot PALETTE = ".palette";

}  // namespace

auto SOUND::VIEWS::SPLIT::measured(GUI::Handle page) -> Seat {
  const GUI::Extent main = GUI::GET::measured(page, MAIN);
  const Float least = GUI::GET::extent(page, BAND).h;
  const Float floor = least + hung(page, roster()[chosen()]);
  return {
    GUI::GET::position(page, MAIN).y + main.h, main.h,
    GUI::GET::extent(page, BAR).h, least, std::max(main.h - floor, SHUT)};
}

auto SOUND::VIEWS::SPLIT::hung(GUI::Handle page, const View &unit) -> Float {
  if (!unit.plate) return SHUT;
  const String grip = String(unit.name) + ::HANDLE;
  const String body = String(unit.name) + ::PALETTE;
  const Float bar = GUI::GET::extent(page, grip.c_str()).h;
  const Float foot = GUI::GET::position(page, body.c_str()).y +
                     GUI::GET::extent(page, body.c_str()).h;
  return std::max(bar, foot);
}

auto SOUND::VIEWS::SPLIT::depth(Whole region) -> Float {
  const Seat seat = measured(sheet());
  const Float tall = std::min(height(), seat.room);
  if (region == BELOW) return std::max(tall - seat.bar - seat.least, SHUT);
  return seat.depth - tall;
}

void SOUND::VIEWS::SPLIT::rooted(
  GUI::Handle page, const Seat &seat, Float tall) {
  const Whole below = lower();
  const Flag whole = shared();
  const Vector<View> &units = roster();
  for (Whole row = 0; row < units.size(); ++row) {
    if (units[row].panel) continue;
    if (below != NONE && row == chosen())
      ::placed(page, units[row].name, SHUT, whole ? SHUT : tall);
    else if (row == below)
      ::placed(page, units[row].name, ::above(seat, tall), SHUT);
    else
      ::placed(page, units[row].name, SHUT, SHUT);
  }
}

auto SOUND::VIEWS::SPLIT::top() -> Float {
  const Seat seat = measured(sheet());
  return ::above(seat, std::min(height(), seat.room));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;

void shown(GUI::Handle page, STRING::Hot id, Flag on) {
  if (GUI::GET::visibility(page, id) != on)
    GUI::set(page, id, GUI::Visibility{on});
}

void laid(GUI::Handle page, STRING::Hot id, Float top, Float depth) {
  const GUI::Position at = GUI::GET::position(page, id);
  const GUI::Extent size = GUI::GET::extent(page, id);
  if (at.y != top) GUI::set(page, id, GUI::Position{at.x, top});
  if (size.h != depth) GUI::set(page, id, GUI::Extent{size.w, depth});
}

auto foot(GUI::Handle page, STRING::Hot id) -> Float {
  return GUI::GET::position(page, id).y + GUI::GET::extent(page, id).h;
}

void rimmed(GUI::Handle page) {
  using namespace VIEWS::SPLIT;
  const Flag below = focused() == BELOW;
  const Float top = below ? GUI::GET::position(page, BAR).y + ::foot(page, BAND)
                          : GUI::GET::position(page, GROUND::UPPER).y;
  const STRING::Hot ground = below ? GROUND::LOWER : GROUND::UPPER;
  ::laid(page, RIM, top, ::foot(page, ground) - top);
}

}  // namespace

void SOUND::VIEWS::SPLIT::grounded(
  GUI::Handle page, const Seat &seat, Float tall) {
  const Flag on = open();
  ::shown(page, GROUND::LOWER, on && !doubled());
  ::shown(page, GROUND::UPPER, on);
  ::shown(page, RIM, on);
  if (!on) return;
  ::laid(page, GROUND::LOWER, seat.foot - tall + seat.bar, tall - seat.bar);
  ::laid(page, GROUND::UPPER, seat.foot - seat.depth, seat.depth - tall);
  ::rimmed(page);
}

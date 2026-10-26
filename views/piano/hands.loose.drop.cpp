// SPDX-License-Identifier: AGPL-3.0-or-later
#include <numeric>

#include <island/gui/carry.hpp>

#include "piano.acts.hpp"

void SOUND::VIEWS::ROLL::dropped(GUI::Handle page) {
  if (!GUI::SAC::CARRY::GET::carried(page).empty()) return;
  Vector<Whole> slots(loose().size());
  std::iota(slots.begin(), slots.end(), Whole{0});
  discard(slots);
  GUI::SAC::CARRY::follow(page, {});
}

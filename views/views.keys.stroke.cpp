// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

void SOUND::VIEWS::draw(const GUI::SAC::STROKE::Mode &mode) {
  if (listening()) GUI::SAC::STROKE::draw(document(), mode);
}

auto SOUND::VIEWS::GET::gesture() -> GUI::SAC::STROKE::Gesture {
  if (!listening()) return {};
  return GUI::SAC::STROKE::GET::gesture(document());
}

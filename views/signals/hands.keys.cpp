// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "signals.internal.hpp"

void SOUND::VIEWS::SIGNALS::keyed() {
  const GUI::Handle page = document();
  const INPUT::KEYS::Event key = GUI::GET::keyed(page);
  if (VIEWS::journaled(key)) return;
  if (key.action != INPUT::KEYS::DELETE) return;
  erase(page, chosen());
}

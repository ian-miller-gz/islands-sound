// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

auto SOUND::VIEWS::elsewhere(STRING::Hot door, STRING::Hot panel) -> Flag {
  const GUI::Handle page = document();
  if (VIEWS::escaped()) return true;
  if (!GUI::GET::pressed(page)) return false;
  const String own(door);
  for (const GUI::Event &event : GUI::GET::events(page)) {
    if (event.kind != GUI::Event::PRESSED) continue;
    const String id(event.id);
    if (!own.empty() && (id == own || id.starts_with(own + "."))) return false;
    if (id.starts_with(panel)) return false;
  }
  return true;
}

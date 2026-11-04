// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using GUI::Event;

Flag gripping = false;

auto ended(const Event &event) -> Flag {
  return event.kind == Event::MOVED || event.kind == Event::CLICKED;
}

}  // namespace

void SOUND::VIEWS::SPLIT::RELAY::watch(GUI::Handle page) {
  for (const Event &event : GUI::GET::events(page)) {
    if (event.id != BAR) continue;
    if (event.kind == Event::PRESSED) ::gripping = true;
    if (::ended(event)) ::gripping = false;
  }
  if (String(GUI::GET::hover(page)) != BAR) ::gripping = false;
}

auto SOUND::VIEWS::SPLIT::RELAY::gripped() -> Flag { return ::gripping; }

auto SOUND::VIEWS::SPLIT::RELAY::deferred() -> Flag {
  return ::gripping && shared();
}

auto SOUND::VIEWS::SPLIT::RELAY::still(Whole row) -> Flag {
  return deferred() && (row == chosen() || row == lower());
}

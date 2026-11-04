// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.menu.acts.hpp"

namespace {
using namespace SOUND;

String parked;

}  // namespace

void SOUND::VIEWS::MENU::park(GUI::Handle page, const String &cell) {
  ::parked = cell;
  GUI::edit(page, "");
}

auto SOUND::VIEWS::MENU::GET::door() -> String { return ::parked; }

void SOUND::VIEWS::MENU::keys() {
  if (::parked.empty()) return;
  const GUI::Handle page = document();
  if (standing() && VIEWS::escaped()) lower();
  if (standing()) return;
  if (GUI::GET::editing(page).empty()) GUI::edit(page, ::parked.c_str(), false);
  ::parked.clear();
}

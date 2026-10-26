// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "views.deck.restore.hpp"

namespace {

constexpr STRING::Hot DOOR = "deck.restore";

}  // namespace

void SOUND::VIEWS::DECK::restore(GUI::Handle page) {
  const Flag standing = !CONTROL::overrides().empty();
  GUI::set(page, ::DOOR, GUI::Visibility{standing});
  if (standing && GUI::GET::clicked(page, ::DOOR)) CONTROL::restore();
}

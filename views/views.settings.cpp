// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "settings";
constexpr STRING::Hot ASK = "settings.ask";
constexpr STRING::Hot OWN = "settings.own";
constexpr STRING::Hot SHADE = "settings.shade";
constexpr STRING::Hot CLOSE = "settings.close";
constexpr STRING::Hot LIT = "loose";
constexpr STRING::Hot PLAIN = "place";
constexpr STRING::Hot FOLLOWS = "Theme follows the launcher";
constexpr STRING::Hot OWNS = "Theme is this application's own";
constexpr STRING::Hot LIGHT = "Light";
constexpr STRING::Hot DARK = "Dark";

GUI::Theme pending = GUI::LIGHT;
Flag flipping = false;

void raised(GUI::Handle page, Flag on) {
  GUI::set(page, ::ASK, GUI::Visibility{on});
}

void worded(GUI::Handle page) {
  const Flag own = VIEWS::owned();
  GUI::set(page, ::OWN, GUI::Text{own ? ::OWNS : ::FOLLOWS});
  GUI::set(page, ::OWN, GUI::Style{own ? ::LIT : ::PLAIN});
  GUI::set(
    page, ::SHADE,
    GUI::Text{GUI::GET::theme() == GUI::DARK ? ::DARK : ::LIGHT});
}

void pressed(GUI::Handle page) {
  if (GUI::GET::clicked(page, ::OWN)) VIEWS::owned(!VIEWS::owned());
  if (GUI::GET::clicked(page, ::SHADE))
    VIEWS::shaded(GUI::GET::theme() == GUI::DARK ? GUI::LIGHT : GUI::DARK);
}

}  // namespace

void SOUND::VIEWS::shaded(GUI::Theme shade) {
  ::pending = shade;
  ::flipping = true;
}

void SOUND::VIEWS::dressed() {
  if (::flipping) {
    const GUI::Handle page = document();
    const Flag standing = GUI::GET::visibility(page, ::ASK);
    GUI::theme(::pending);
    if (standing) ::raised(page, true);
  }
  ::flipping = false;
  remembered();
}

auto SOUND::VIEWS::asking() -> Flag {
  return GUI::GET::visibility(document(), ::ASK);
}

void SOUND::VIEWS::settings() {
  const GUI::Handle page = document();
  const Flag standing = GUI::GET::visibility(page, ::ASK);
  if (GUI::GET::clicked(page, ::DOOR)) return ::raised(page, !standing);
  if (!standing) return;
  ::pressed(page);
  ::worded(page);
  if (
    GUI::GET::clicked(page, ::CLOSE) ||
    INPUT::GET::pressed(INPUT::KEYS::ESCAPE) || VIEWS::elsewhere(::DOOR, ::ASK))
    ::raised(page, false);
}

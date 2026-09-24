// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot TOGGLE = "arrangement.snap";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";

Flag standing = true;

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::snapped() -> Flag { return ::standing; }

void SOUND::VIEWS::ARRANGEMENT::snap() {
  const GUI::Handle page = VIEWS::document();
  if (GUI::GET::clicked(page, ::TOGGLE)) ::standing = !::standing;
  GUI::set(page, ::TOGGLE, GUI::Style{::standing ? ::MARKED : ::PLAIN});
}

void SOUND::VIEWS::ARRANGEMENT::snap(SHELL::Session &session) {
  session.print(std::format("snap {}", ::standing ? "on" : "off"));
}

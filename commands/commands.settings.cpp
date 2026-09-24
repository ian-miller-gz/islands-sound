// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../views.hpp"

namespace {
constexpr STRING::Hot OWN = "own";
constexpr STRING::Hot SHADE = "shade";
constexpr STRING::Hot ON = "on";
constexpr STRING::Hot OFF = "off";
constexpr STRING::Hot NIGHT = "dark";
constexpr STRING::Hot DAY = "light";
constexpr STRING::Hot USAGE = "settings [own on|off | shade light|dark]";
}  // namespace

void SOUND::COMMANDS::settings(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() == 1) return VIEWS::settings(session);
  if (words.size() != 3) return session.print(::USAGE);
  if (words[1] == ::OWN && (words[2] == ::ON || words[2] == ::OFF)) {
    VIEWS::owned(words[2] == ::ON);
    return VIEWS::settings(session);
  }
  if (words[1] == ::SHADE && (words[2] == ::NIGHT || words[2] == ::DAY)) {
    VIEWS::shaded(words[2] == ::NIGHT ? GUI::DARK : GUI::LIGHT);
    return session.print("shading " + words[2]);
  }
  session.print(::USAGE);
}

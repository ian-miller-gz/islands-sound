// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot OWNED = "theme";
constexpr STRING::Hot SHADED = "shade";
constexpr STRING::Hot OWN = "own";
constexpr STRING::Hot LAUNCHED = "launcher";
constexpr STRING::Hot NIGHT = "dark";
constexpr STRING::Hot DAY = "light";
constexpr STRING::Hot UP = "up";
constexpr STRING::Hot DOWN = "down";
constexpr STRING::Hot LAUNCHER = "configs/launcher.yaml";
constexpr STRING::Hot PATH = "configs/sound.yaml";
constexpr STRING::Hot HEADING =
  "# The sound bundle's own settings, written by its settings plate.\n"
  "# theme: whose the dress is - launcher (the launcher's last dress is\n"
  "#   worn at every start) or own (the shade below is).\n"
  "# shade: the dress worn while the theme is the bundle's own.\n";

Flag own = false;
String worn;

auto named(GUI::Theme shade) -> String {
  return shade == GUI::DARK ? NIGHT : DAY;
}

auto lined(STRING::Hot path, STRING::Hot wanted) -> String {
  IO::STREAMS::Input in(path);
  String line, key, value;
  while (std::getline(in, line))
    if (STRING::pair(line, key, value) && key == wanted) return value;
  return {};
}

void written() {
  IO::STREAMS::Output out(::PATH);
  if (!out) return;
  out << ::HEADING << ::OWNED << ": " << (::own ? ::OWN : ::LAUNCHED) << "\n"
      << ::SHADED << ": " << ::worn << "\n";
}

void wear(const String &shade) {
  if (shade == ::NIGHT) GUI::theme(GUI::DARK);
  if (shade == ::DAY) GUI::theme(GUI::LIGHT);
}

}  // namespace

auto SOUND::VIEWS::owned() -> Flag { return ::own; }

void SOUND::VIEWS::owned(Flag on) {
  ::own = on;
  ::written();
}

void SOUND::VIEWS::settled() {
  ::own = ::lined(::PATH, ::OWNED) == ::OWN;
  ::wear(::own ? ::lined(::PATH, ::SHADED) : ::lined(::LAUNCHER, ::OWNED));
  ::worn = ::named(GUI::GET::theme());
}

void SOUND::VIEWS::remembered() {
  const String now = ::named(GUI::GET::theme());
  if (now == ::worn) return;
  ::worn = now;
  if (::own) ::written();
}

void SOUND::VIEWS::settings(SHELL::Session &session) {
  session.print(std::format(
    "settings theme {} shade {} plate {}", ::own ? ::OWN : ::LAUNCHED,
    ::named(GUI::GET::theme()), VIEWS::asking() ? ::UP : ::DOWN));
}

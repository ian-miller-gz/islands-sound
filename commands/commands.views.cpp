// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../views.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto controls(const VIEWS::View &unit) -> String {
  if (unit.controls.empty()) return "none";
  String said;
  for (STRING::Hot control : unit.controls)
    said += (said.empty() ? "" : " ") + String(control);
  return said;
}

auto keys() -> STRING::Hot {
  return GUI::GET::document() == VIEWS::document() ? "held" : "loose";
}

auto standing(const VIEWS::View &unit) -> STRING::Hot {
  const Flag on = unit.panel ? VIEWS::raised(unit.name)
                             : String(unit.name) == VIEWS::standing();
  return on ? "shown" : "hidden";
}

void stated(SHELL::Session &session, const VIEWS::View &unit) {
  session.print(std::format(
    "view {} {} {} controls {}", unit.name, unit.panel ? "panel" : "main",
    ::standing(unit), ::controls(unit)));
  if (unit.state != nullptr) unit.state(session);
}

}  // namespace

void SOUND::COMMANDS::views(SHELL::Session &session) {
  session.print(std::format(
    "views {} main {} keys {}", VIEWS::roster().size(), VIEWS::standing(),
    ::keys()));
  for (const VIEWS::View &unit : VIEWS::roster())
    session.print(std::format(
      "view {} {} {} controls {}", unit.name, unit.panel ? "panel" : "main",
      ::standing(unit), ::controls(unit)));
}

void SOUND::COMMANDS::view(SHELL::Session &session) {
  if (session.arguments.size() < 2) {
    const VIEWS::View *unit = VIEWS::found(String(VIEWS::standing()));
    if (unit == nullptr) return session.print("no view stands");
    return ::stated(session, *unit);
  }
  const VIEWS::View *unit = VIEWS::found(session.arguments[1]);
  if (unit == nullptr) return session.print("no such view");
  VIEWS::choose(session.arguments[1]);
  ::stated(session, *unit);
}

void SOUND::COMMANDS::panel(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("panel <name> <on|off>");
  if (!VIEWS::raise(session.arguments[1], session.arguments[2] == "on"))
    return session.print("panel refused: no such panel");
  const VIEWS::View *unit = VIEWS::found(session.arguments[1]);
  session.print(std::format(
    "panel {} {}", unit->name, VIEWS::raised(unit->name) ? "shown" : "hidden"));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../views.hpp"
#include "../commands.hpp"

namespace {

constexpr STRING::Hot WHOLE = "none";

auto measured(const String &token) -> Float {
  return std::strtof(token.c_str(), nullptr);
}

}  // namespace

void SOUND::COMMANDS::zoom(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("zoom <across> [<down>]");
  const Flag paired = session.arguments.size() > 2;
  const Float across = ::measured(session.arguments[1]);
  const Float down = paired ? ::measured(session.arguments[2]) : across;
  if (!VIEWS::zoom(across, down))
    return session.print(
      std::format("zoom refused: {} has no scale", VIEWS::standing()));
  session.print(std::format(
    "zoom {} {}{}", VIEWS::standing(), session.arguments[1],
    paired ? " " + session.arguments[2] : String()));
}

void SOUND::COMMANDS::pan(SHELL::Session &session) {
  if (session.arguments.size() < 3) return session.print("pan <x> <y>");
  if (!VIEWS::pan(
        ::measured(session.arguments[1]), ::measured(session.arguments[2])))
    return session.print(
      std::format("pan refused: {} has no view offset", VIEWS::standing()));
  session.print(std::format(
    "pan {} {} {}", VIEWS::standing(), session.arguments[1],
    session.arguments[2]));
}

void SOUND::COMMANDS::look(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("look <node>|none");
  const String &token = session.arguments[1];
  if (token == ::WHOLE) {
    CONTROL::aim({});
    return session.print("looking at the whole graph");
  }
  if (GRAPH::at(token) == NONE)
    return session.print("look refused: no such node");
  CONTROL::aim(token);
  session.print(
    std::format("looking at node {} {}", token, VIEWS::named(token)));
}

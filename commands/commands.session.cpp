// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../views.hpp"
#include "../commands.hpp"

using namespace SOUND;

namespace {

constexpr STRING::Hot SPACE = " ";

auto named(const SHELL::Session &session) -> String {
  String name;
  for (Whole word = 1; word < session.arguments.size(); ++word)
    name += (name.empty() ? "" : ::SPACE) + session.arguments[word];
  return name;
}

}  // namespace

void SOUND::COMMANDS::document(SHELL::Session &session) {
  session.print(std::format(
    "session {} {} tracks {} stocks {} nodes {} recents",
    SESSION::held().name.empty() ? "unnamed" : SESSION::held().name.c_str(),
    TIMELINE::held().tracks.size(), INVENTORY::held().stocks.size(),
    GRAPH::held().nodes.size(), SESSION::held().recents.size()));
}

auto SOUND::COMMANDS::save(const String &name) -> String {
  INVENTORY::house(name);
  if (!SESSION::save(
        name, TRANSPORT::held(), TIMELINE::held(), INVENTORY::held(),
        GRAPH::held(), CONTROL::held(), HISTORY::held()))
    return "save refused";
  SESSION::keep(SESSION::held().name, VIEWS::kept());
  SESSION::forget(SESSION::held().name);
  settled();
  return std::format("saved {}", SESSION::held().name);
}

void SOUND::COMMANDS::save(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("save <name>");
  session.print(save(::named(session)));
}

void SOUND::COMMANDS::husks(SHELL::Session &session) {
  const Vector<Husk> held = SESSION::husks();
  session.print(std::format("husks {}", held.size()));
  for (const Husk &husk : held)
    session.print(std::format(
      "{} {}", husk.half.empty() ? String("row") : husk.half, husk.line));
}

void SOUND::COMMANDS::saved(SHELL::Session &session) {
  const Vector<String> kept = SESSION::names();
  session.print(std::format("sessions {}", kept.size()));
  for (const String &name : kept) session.print(name);
}

void SOUND::COMMANDS::open(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("open <name>");
  const String said = open(::named(session));
  for (Whole from = 0, at = 0; at <= said.size(); ++at)
    if (at == said.size() || said[at] == '\n') {
      session.print(said.substr(from, at - from));
      from = at + 1;
    }
}

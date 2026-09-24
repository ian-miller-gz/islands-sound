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

void SOUND::COMMANDS::document(SHELL::Session &session) {
  session.print(std::format(
    "session {} {} tracks {} stocks {} nodes {} recents",
    SESSION::held().name.empty() ? "unnamed" : SESSION::held().name.c_str(),
    TIMELINE::held().tracks.size(), INVENTORY::held().stocks.size(),
    GRAPH::held().nodes.size(), SESSION::held().recents.size()));
}

auto SOUND::COMMANDS::save(const String &name) -> String {
  if (!SESSION::save(
        name, TRANSPORT::held(), TIMELINE::held(), INVENTORY::held(),
        GRAPH::held(), CONTROL::held(), HISTORY::held()))
    return "save refused";
  SESSION::keep(SESSION::held().name, VIEWS::kept());
  return std::format("saved {}", SESSION::held().name);
}

void SOUND::COMMANDS::save(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("save <name>");
  session.print(save(session.arguments[1]));
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

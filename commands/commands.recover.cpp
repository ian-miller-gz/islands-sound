// SPDX-License-Identifier: AGPL-3.0-or-later
#include <chrono>
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;
using Clock = std::chrono::steady_clock;

constexpr STRING::Hot UNNAMED = "unnamed";
constexpr auto PERIOD = std::chrono::seconds(1);

Whole written = NONE;
Clock::time_point stamp;

auto named() -> String {
  const String &name = SESSION::held().name;
  return name.empty() ? String(::UNNAMED) : name;
}

// The counted edits: the ledger's and the inventory's, RENDER::moved's own
// sum — what a Save would be saving.
auto moved() -> Whole { return HISTORY::counted() + INVENTORY::counted(); }

// The row written: the housed takes first, so a recorded take the row
// names by path is on disk before the row is.
void write() {
  INVENTORY::house(SESSION::held().name);
  SESSION::recover(
    SESSION::held().name, TRANSPORT::held(), TIMELINE::held(),
    INVENTORY::held(), GRAPH::held(), CONTROL::held(), HISTORY::held());
  ::written = ::moved();
  ::stamp = Clock::now();
}

}  // namespace

void SOUND::COMMANDS::settled() {
  ::written = ::moved();
  ::stamp = Clock::now();
}

void SOUND::COMMANDS::checkpoint() {
  if (::moved() == ::written) return;
  if (Clock::now() - ::stamp < ::PERIOD) return;
  ::write();
}

void SOUND::COMMANDS::checkpoint(SHELL::Session &session) {
  ::write();
  session.print(std::format("checkpoint {} edits {}", ::named(), ::written));
}

void SOUND::COMMANDS::recovery(SHELL::Session &session) {
  const Flag standing = SESSION::recovered(SESSION::held().name);
  session.print(std::format(
    "recovery {} {}", ::named(), standing ? "standing" : "none"));
}

void SOUND::COMMANDS::recover(SHELL::Session &session) {
  const String said = recover(SESSION::held().name);
  for (Whole from = 0, at = 0; at <= said.size(); ++at)
    if (at == said.size() || said[at] == '\n') {
      session.print(said.substr(from, at - from));
      from = at + 1;
    }
}

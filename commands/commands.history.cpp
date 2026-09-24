// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../history.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto ledger() -> String {
  return std::format(
    "history {} undone {}", HISTORY::held().edits.size(),
    HISTORY::held().undone);
}

}  // namespace

void SOUND::COMMANDS::history(SHELL::Session &session) {
  session.print(::ledger());
  const Vector<Edit> &edits = HISTORY::held().edits;
  for (Whole index = 0; index < edits.size(); ++index) {
    const Edit &edit = edits[index];
    session.print(lined(index, edit));
    for (const String &name : edit.swept)
      session.print(std::format("gone {}", name));
  }
}

void SOUND::COMMANDS::undo(SHELL::Session &session) {
  const String said = walked(session, true);
  session.print(said);
  if (!said.starts_with("undo refused")) session.print(::ledger());
}

void SOUND::COMMANDS::redo(SHELL::Session &session) {
  const String said = walked(session, false);
  session.print(said);
  if (!said.starts_with("redo refused")) session.print(::ledger());
}

auto SOUND::COMMANDS::undo() -> String {
  Gathered quiet;
  return walked(quiet, true);
}

auto SOUND::COMMANDS::redo() -> String {
  Gathered quiet;
  return walked(quiet, false);
}

void SOUND::COMMANDS::trim(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("trim <keep>");
  const Whole standing = HISTORY::held().edits.size();
  HISTORY::trim(::counted(session.arguments[1]));
  session.print(
    std::format("trimmed {}", standing - HISTORY::held().edits.size()));
  session.print(::ledger());
}

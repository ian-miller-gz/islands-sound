// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../views.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;
namespace HOTKEYS = SOUND::VIEWS::HOTKEYS;

constexpr STRING::Hot COMMAND = "hotkeys";
constexpr STRING::Hot HELP = "";
constexpr STRING::Hot COUNT = "hotkeys {}";
constexpr STRING::Hot LINE = "hotkey {} {} {}";

void hotkeys(SHELL::Session &session) {
  const Vector<HOTKEYS::Row> rows = HOTKEYS::listed();
  session.print(std::format(::COUNT, rows.size()));
  for (const HOTKEYS::Row &row : rows)
    session.print(
      std::format(::LINE, row.family, row.act, HOTKEYS::spelled(row.chord)));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::COMMAND, ::HELP, ::hotkeys}});

}  // namespace

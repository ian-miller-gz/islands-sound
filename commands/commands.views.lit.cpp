// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../views.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WORD = "lit";
constexpr STRING::Hot HELP = "lists lit icons";
constexpr STRING::Hot LINE = "lit {} {}";

void lit(SHELL::Session &session) {
  for (const VIEWS::LIT::Row &row : VIEWS::lit())
    session.print(std::format(::LINE, row.id, row.style));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::WORD, ::HELP, ::lit}});

}  // namespace

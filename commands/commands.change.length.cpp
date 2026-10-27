// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../inventory.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr Integer BASE = 10;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::lengthened(const Change &change) -> String {
  const Inventory &pool = INVENTORY::held();
  const Whole stock = ::counted(change.at);
  if (stock >= pool.stocks.size()) return {};
  return std::format("{}", pool.stocks[stock].length);
}

auto SOUND::COMMANDS::CHANGE::lengthened(
  const Change &change, const String &value) -> Flag {
  return INVENTORY::length(::counted(change.at), ::counted(value));
}

auto SOUND::COMMANDS::lengthened(Whole stock, Whole length) -> Flag {
  if (stock >= INVENTORY::held().stocks.size()) return false;
  return kept(
    {.word = HISTORY::WORD::LENGTH,
     .at = std::format("{}", stock),
     .now = std::format("{}", length)});
}

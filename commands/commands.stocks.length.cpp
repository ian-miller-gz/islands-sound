// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../inventory.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot USAGE = "length <stock> [<length>]";
constexpr STRING::Hot REFUSED = "length refused: no such stock";
constexpr STRING::Hot LINE = "length stock {} {} content {}";
constexpr Integer BASE = 10;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

}  // namespace

void SOUND::COMMANDS::length(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print(::USAGE);
  const Whole stock = ::counted(session.arguments[1]);
  const Inventory &pool = INVENTORY::held();
  if (stock >= pool.stocks.size()) return session.print(::REFUSED);
  if (
    session.arguments.size() > 2 &&
    !lengthened(stock, ::counted(session.arguments[2])))
    return session.print(::REFUSED);
  const Stock &held = pool.stocks[stock];
  session.print(std::format(
    ::LINE, stock, INVENTORY::length(held), INVENTORY::carried(held)));
}

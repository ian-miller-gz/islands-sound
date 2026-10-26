// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <sstream>

#include "../inventory.hpp"
#include "commands.aim.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr Integer BASE = 10;

auto stocked(const Change &change) -> const Stock * {
  const Whole stock = std::strtoul(change.at.c_str(), nullptr, ::BASE);
  const Inventory &pool = INVENTORY::held();
  if (change.at.empty() || stock >= pool.stocks.size()) return nullptr;
  return pool.stocks[stock].kind == KIND::LOGIC ? &pool.stocks[stock] : nullptr;
}

auto spoken(const Address &address) -> String {
  if (address.node.empty()) return {};
  return std::format("{} {}", address.node, address.parameter);
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::aimed(const Change &change) -> String {
  const Stock *held = ::stocked(change);
  return held == nullptr ? String() : ::spoken(held->curve.address);
}

auto SOUND::COMMANDS::CHANGE::aimed(const Change &change, const String &value)
  -> Flag {
  if (::stocked(change) == nullptr) return false;
  Address address;
  std::istringstream said(value);
  said >> address.node >> address.parameter;
  if (address.node.empty()) address = {};
  return INVENTORY::aim(
    std::strtoul(change.at.c_str(), nullptr, ::BASE), address);
}

auto SOUND::COMMANDS::aimed(Whole stock, const Address &address) -> Flag {
  return kept(
    {.word = HISTORY::WORD::AIM,
     .at = std::to_string(stock),
     .now = ::spoken(address)});
}

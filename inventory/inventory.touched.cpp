// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "inventory.internal.hpp"

namespace {

SOUND::Touched held;
Whole counted = 0;
Whole all = 0;
Vector<Whole> stirs;

}  // namespace

auto SOUND::INVENTORY::counted() -> Whole { return ::counted; }

auto SOUND::INVENTORY::stirred(Whole stock) -> Whole {
  return stock < ::stirs.size() ? std::max(::stirs[stock], ::all) : ::all;
}

auto SOUND::INVENTORY::touched() -> const Touched & { return ::held; }

void SOUND::INVENTORY::settled() { ::held = {}; }

void SOUND::INVENTORY::stir() {
  ++::counted;
  ::all = ::counted;
  ::held.whole = true;
}

void SOUND::INVENTORY::stir(Whole stock) {
  ++::counted;
  if (stock >= ::stirs.size()) ::stirs.resize(stock + 1, 0);
  ::stirs[stock] = ::counted;
  Vector<Whole> &stocks = ::held.stocks;
  if (std::find(stocks.begin(), stocks.end(), stock) == stocks.end())
    stocks.push_back(stock);
}

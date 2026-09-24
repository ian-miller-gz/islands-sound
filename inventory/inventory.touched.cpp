// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "inventory.internal.hpp"

namespace {

SOUND::Touched held;
Whole counted = 0;

}  // namespace

auto SOUND::INVENTORY::counted() -> Whole { return ::counted; }

auto SOUND::INVENTORY::touched() -> const Touched & { return ::held; }

void SOUND::INVENTORY::settled() { ::held = {}; }

void SOUND::INVENTORY::stir() {
  ++::counted;
  ::held.whole = true;
}

void SOUND::INVENTORY::stir(Whole stock) {
  ++::counted;
  Vector<Whole> &stocks = ::held.stocks;
  if (std::find(stocks.begin(), stocks.end(), stock) == stocks.end())
    stocks.push_back(stock);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

auto SOUND::INVENTORY::name(Whole stock, const String &name) -> Flag {
  Inventory &pool = standing();
  if (stock >= pool.stocks.size() || name.empty()) return false;
  pool.stocks[stock].name = name;
  stir(stock);
  return true;
}

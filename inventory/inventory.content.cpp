// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

auto SOUND::INVENTORY::content(Whole stock, const Stock &clip) -> Flag {
  Inventory &pool = standing();
  if (stock >= pool.stocks.size()) return false;
  Stock &held = pool.stocks[stock];
  held.score = clip.score;
  held.curve = clip.curve;
  held.turns = clip.turns;
  held.choices = clip.choices;
  stir(stock);
  return true;
}

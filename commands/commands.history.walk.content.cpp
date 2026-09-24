// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../history.hpp"
#include "../inventory.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

void named(Vector<Whole> &stocks, Whole stock) {
  for (const Whole one : stocks)
    if (one == stock) return;
  stocks.push_back(stock);
}

}  // namespace

void SOUND::COMMANDS::rewrote(const Edit &edit, Flag back) {
  Vector<Whole> stocks;
  for (const Noted &one : edit.notes) ::named(stocks, one.stock);
  for (const Plotted &one : edit.plots) ::named(stocks, one.stock);
  for (const Whole stock : stocks) {
    if (stock >= INVENTORY::held().stocks.size()) continue;
    Stock held = INVENTORY::held().stocks[stock];
    HISTORY::restored(edit, back, stock, held);
    INVENTORY::content(stock, held);
  }
}

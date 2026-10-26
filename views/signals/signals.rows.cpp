// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "signals.runs.hpp"

namespace {
using namespace SOUND;

auto stocked(Whole index) -> const Stock * {
  const Inventory &pool = INVENTORY::held();
  return index < pool.stocks.size() && VIEWS::SIGNALS::RUNS::paged() != nullptr
           ? &pool.stocks[index]
           : nullptr;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::row(Whole stock, Whole index) -> Row {
  const Stock *held = ::stocked(stock);
  return held == nullptr ? Row{} : RUNS::paged()->row(*held, index);
}

auto SOUND::VIEWS::SIGNALS::covered(Whole stock, Whole at) -> Whole {
  const Stock *held = ::stocked(stock);
  return held == nullptr ? NONE : RUNS::paged()->covered(*held, at);
}

auto SOUND::VIEWS::SIGNALS::clear(Whole stock, Whole opens, Whole closes)
  -> Whole {
  const Stock *held = ::stocked(stock);
  if (held == nullptr) return 0;
  const Vector<Whole> rows = RUNS::paged()->between(*held, opens, closes);
  for (Whole row = rows.size(); row > 0; --row)
    INVENTORY::erase(stock, rows[row - 1]);
  return rows.size();
}

auto SOUND::VIEWS::SIGNALS::write(Whole stock, const Row &laid) -> Flag {
  const Stock *held = ::stocked(stock);
  return held != nullptr && RUNS::paged()->write(stock, laid);
}

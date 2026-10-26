// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto stocked(Whole index) -> const Stock * {
  const Inventory &pool = INVENTORY::held();
  return index < pool.stocks.size() ? &pool.stocks[index] : nullptr;
}

auto between(Whole stock, Whole opens, Whole closes) -> Vector<Whole> {
  Vector<Whole> rows;
  const Stock *held = ::stocked(stock);
  if (held == nullptr || VIEWS::SIGNALS::paged() != KIND::CONTROL) return rows;
  for (Whole index = 0; index < held->turns.size(); ++index)
    if (
      held->turns[index].number == VIEWS::SIGNALS::number() &&
      held->turns[index].at > opens && held->turns[index].at < closes)
      rows.push_back(index);
  return rows;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::row(Whole stock, Whole index) -> Row {
  const Stock *held = ::stocked(stock);
  if (held == nullptr) return {};
  if (paged() == KIND::CONTROL)
    return index < held->turns.size()
             ? Row{
                 held->turns[index].at, held->turns[index].value,
                 held->turns[index].shape}
             : Row{};
  return index < held->choices.size()
           ? Row{held->choices[index].at, Float(held->choices[index].program)}
           : Row{};
}

auto SOUND::VIEWS::SIGNALS::covered(Whole stock, Whole at) -> Whole {
  const Stock *held = ::stocked(stock);
  if (held == nullptr) return NONE;
  if (paged() != KIND::CONTROL) {
    for (Whole index = 0; index < held->choices.size(); ++index)
      if (held->choices[index].at == at) return index;
    return NONE;
  }
  for (Whole index = 0; index < held->turns.size(); ++index)
    if (held->turns[index].number == number() && held->turns[index].at == at)
      return index;
  return NONE;
}

auto SOUND::VIEWS::SIGNALS::clear(Whole stock, Whole opens, Whole closes)
  -> Whole {
  const Vector<Whole> rows = ::between(stock, opens, closes);
  for (Whole row = rows.size(); row > 0; --row)
    INVENTORY::erase(stock, rows[row - 1]);
  return rows.size();
}

auto SOUND::VIEWS::SIGNALS::write(Whole stock, const Row &laid) -> Flag {
  if (paged() == KIND::CONTROL)
    return INVENTORY::turn(
      stock, {.at = laid.at,
              .number = number(),
              .value = laid.value,
              .shape = laid.shape});
  return INVENTORY::choose(
    stock, {.at = laid.at, .program = Whole(laid.value)});
}

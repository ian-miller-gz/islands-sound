// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

namespace {
using namespace SOUND;

template <typename Row>
auto shaped(Vector<Row> &rows, Whole row, Whole shape) -> Flag {
  if (row >= rows.size()) return false;
  rows[row].shape = shape;
  return true;
}

template <typename Row>
auto shaped(const Vector<Row> &rows, Whole row) -> Whole {
  return row < rows.size() ? rows[row].shape : NONE;
}

}  // namespace

auto SOUND::INVENTORY::shape(Whole stock, Whole row, Whole shape) -> Flag {
  if (stock >= standing().stocks.size()) return false;
  Stock &held = standing().stocks[stock];
  const Flag laid =
    held.kind == KIND::CONTROL ? ::shaped(held.turns, row, shape)
    : held.kind == KIND::LOGIC ? ::shaped(held.curve.points, row, shape)
                               : false;
  if (laid) stir(stock);
  return laid;
}

auto SOUND::INVENTORY::shape(Whole stock, Whole row) -> Whole {
  if (stock >= standing().stocks.size()) return NONE;
  const Stock &held = standing().stocks[stock];
  if (held.kind == KIND::CONTROL) return ::shaped(held.turns, row);
  if (held.kind == KIND::LOGIC) return ::shaped(held.curve.points, row);
  return NONE;
}

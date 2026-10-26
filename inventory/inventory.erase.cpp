// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

namespace {
using namespace SOUND;

template <typename Row>
auto taken(Vector<Row> &rows, Whole row) -> Flag {
  if (row >= rows.size()) return false;
  rows.erase(rows.begin() + static_cast<Integer>(row));
  return true;
}

struct Run {
  Whole kind = NONE;
  auto (*erase)(Stock &stock, Whole row) -> Flag = nullptr;
};

constexpr Run RUNS[] = {
  {KIND::CONTROL,
   [](Stock &stock, Whole row) { return ::taken(stock.turns, row); }},
  {KIND::PROGRAM,
   [](Stock &stock, Whole row) { return ::taken(stock.choices, row); }},
  {KIND::LOGIC,
   [](Stock &stock, Whole row) { return ::taken(stock.curve.points, row); }}};

}  // namespace

auto SOUND::INVENTORY::erase(Whole stock, Whole row) -> Flag {
  if (stock >= held().stocks.size()) return false;
  Stock &holding = standing().stocks[stock];
  for (const ::Run &run : ::RUNS)
    if (run.kind == holding.kind && run.erase(holding, row)) {
      stir(stock);
      return true;
    }
  return false;
}

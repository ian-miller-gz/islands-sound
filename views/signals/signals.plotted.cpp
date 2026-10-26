// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../shape.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;

auto curved(Whole index) -> const Stock * {
  const Inventory &pool = INVENTORY::held();
  if (index >= pool.stocks.size()) return nullptr;
  return pool.stocks[index].kind == KIND::LOGIC ? &pool.stocks[index] : nullptr;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::addressed() -> Address {
  for (const Span &span : spans()) {
    const Stock *held = ::curved(span.stock);
    if (held != nullptr && !held->curve.address.node.empty())
      return held->curve.address;
  }
  return {};
}

auto SOUND::VIEWS::SIGNALS::plotted(
  const Stock &stock,
  const ARRANGEMENT::TRACK::LANE::Clip &span) -> Vector<Point> {
  Vector<Point> wanted;
  const Vector<SOUND::Point> &run = stock.curve.points;
  for (Whole row = 0; row < run.size(); ++row) {
    const SOUND::Point &node = run[row];
    if (node.at < span.from || node.at - span.from >= span.frames) continue;
    wanted.push_back(
      {span.stock, row, span.at + node.at - span.from, 0, crossed(node.value),
       node.shape, node.bend});
  }
  return wanted;
}

void SOUND::VIEWS::SIGNALS::plotted(SHELL::Session &session) {
  if (paged() != KIND::LOGIC) return;
  for (const Span &span : spans()) {
    const Stock *held = ::curved(span.stock);
    if (held == nullptr) continue;
    const Vector<SOUND::Point> &run = held->curve.points;
    for (Whole row = 0; row < run.size(); ++row)
      session.print(std::format(
        "point {} {} at {} value {:g} shape {}", span.stock, row, run[row].at,
        run[row].value, SHAPE::spoken(run[row].shape)));
  }
}

auto SOUND::VIEWS::SIGNALS::CURVE::row(const Stock &stock, Whole index) -> Row {
  if (index >= stock.curve.points.size()) return {};
  const SOUND::Point &node = stock.curve.points[index];
  return {node.at, crossed(node.value), node.shape};
}

auto SOUND::VIEWS::SIGNALS::CURVE::covered(const Stock &stock, Whole at)
  -> Whole {
  for (Whole index = 0; index < stock.curve.points.size(); ++index)
    if (stock.curve.points[index].at == at) return index;
  return NONE;
}

auto SOUND::VIEWS::SIGNALS::CURVE::write(Whole stock, const Row &laid) -> Flag {
  return INVENTORY::plot(
    stock,
    {.at = laid.at, .value = delivered(laid.value), .shape = laid.shape});
}

auto SOUND::VIEWS::SIGNALS::CURVE::between(
  const Stock &stock, Whole opens, Whole closes) -> Vector<Whole> {
  Vector<Whole> rows;
  for (Whole index = 0; index < stock.curve.points.size(); ++index)
    if (
      stock.curve.points[index].at > opens &&
      stock.curve.points[index].at < closes)
      rows.push_back(index);
  return rows;
}

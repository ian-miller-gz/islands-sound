// SPDX-License-Identifier: AGPL-3.0-or-later
#include <vector>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto turned(const Stock &stock, const ARRANGEMENT::TRACK::LANE::Clip &span)
  -> Vector<VIEWS::SIGNALS::Point> {
  Vector<VIEWS::SIGNALS::Point> wanted;
  const Whole paged = VIEWS::SIGNALS::number();
  for (Whole row = 0; row < stock.turns.size(); ++row) {
    const Turn &move = stock.turns[row];
    if (move.number != paged) continue;
    if (move.at < span.from || move.at - span.from >= span.frames) continue;
    wanted.push_back(
      {span.stock, row, span.at + move.at - span.from, 0, move.value,
       move.shape});
  }
  return wanted;
}

auto chosen(const Stock &stock, const ARRANGEMENT::TRACK::LANE::Clip &span)
  -> Vector<VIEWS::SIGNALS::Point> {
  Vector<VIEWS::SIGNALS::Point> wanted;
  for (Whole row = 0; row < stock.choices.size(); ++row) {
    const Choice &choice = stock.choices[row];
    if (choice.at < span.from || choice.at - span.from >= span.frames) continue;
    wanted.push_back(
      {span.stock, row, span.at + choice.at - span.from, 0,
       Float(choice.program)});
  }
  return wanted;
}

auto gathered(const ARRANGEMENT::TRACK::LANE::Clip &span)
  -> Vector<VIEWS::SIGNALS::Point> {
  const Inventory &pool = INVENTORY::held();
  if (span.stock >= pool.stocks.size()) return {};
  const Stock &stock = pool.stocks[span.stock];
  return VIEWS::SIGNALS::paged() == KIND::CONTROL ? ::turned(stock, span)
                                                  : ::chosen(stock, span);
}

void stands(Vector<VIEWS::SIGNALS::Point> &wanted, Whole closes) {
  for (Whole row = 0; row < wanted.size(); ++row)
    wanted[row].until = row + 1 < wanted.size() ? wanted[row + 1].at : closes;
}

void trimmed(
  Vector<VIEWS::SIGNALS::Point> &wanted, const VIEWS::SIGNALS::Span &lit) {
  if (lit.closes <= lit.opens) return;
  std::erase_if(wanted, [&lit](const VIEWS::SIGNALS::Point &row) {
    return row.at >= lit.closes ||
           (row.until <= lit.opens && row.at < lit.opens);
  });
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::gather(const Span &lit) -> Vector<Point> {
  Vector<Point> wanted;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(attended(), laned())) {
    Vector<Point> rows = ::gathered(span);
    ::stands(rows, span.at + span.frames);
    ::trimmed(rows, lit);
    wanted.insert(wanted.end(), rows.begin(), rows.end());
  }
  return wanted;
}

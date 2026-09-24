// SPDX-License-Identifier: AGPL-3.0-or-later
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

void stands(Vector<VIEWS::SIGNALS::Point> &wanted, Whole closes) {
  for (Whole row = 0; row < wanted.size(); ++row)
    wanted[row].until = row + 1 < wanted.size() ? wanted[row + 1].at : closes;
}

void trimmed(
  Vector<VIEWS::SIGNALS::Point> &wanted, const VIEWS::SIGNALS::Span &lit) {
  if (lit.closes <= lit.opens) return;
  Vector<VIEWS::SIGNALS::Point> kept;
  for (const VIEWS::SIGNALS::Point &row : wanted)
    if (row.at < lit.closes && (row.until > lit.opens || row.at >= lit.opens))
      kept.push_back(row);
  wanted = kept;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::points() -> Vector<Point> {
  Vector<Point> wanted;
  const Inventory &pool = INVENTORY::held();
  const Whole kind = paged();
  const Span lit = seen();
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes) {
      if (lane.kind != kind) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips) {
        if (span.stock >= pool.stocks.size()) continue;
        const Stock &stock = pool.stocks[span.stock];
        Vector<Point> rows =
          kind == KIND::CONTROL ? ::turned(stock, span) : ::chosen(stock, span);
        ::stands(rows, span.at + span.frames);
        ::trimmed(rows, lit);
        wanted.insert(wanted.end(), rows.begin(), rows.end());
      }
    }
  return wanted;
}

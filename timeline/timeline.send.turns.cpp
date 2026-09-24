// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../shape.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

auto ahead(const Vector<Turn> &turns, Whole number, Whole row) -> Whole {
  for (Whole next = row + 1; next < turns.size(); ++next)
    if (turns[next].number == number) return next;
  return NONE;
}

}  // namespace

void SOUND::TIMELINE::turned(
  Vector<Sent> &run, const Stock &stock, const Sending &where,
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole grain) {
  const Whole carried = INVENTORY::length(stock);
  for (Whole row = 0; row < stock.turns.size(); ++row) {
    const Turn &move = stock.turns[row];
    const Whole next = ::ahead(stock.turns, move.number, row);
    const Flag held = next == NONE || move.shape == SHAPE::HOLD || grain == 0;
    const Whole ends = held ? move.at + 1 : stock.turns[next].at;
    for (Whole at = move.at; at < ends; at += held ? 1 : grain)
      stamp(
        run, where, span, carried,
        {.kind = KIND::CONTROL,
         .at = at,
         .number = move.number,
         .value = valued(stock.turns, move.number, at)});
  }
}

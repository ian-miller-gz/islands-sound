// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../shape.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

auto ahead(const Vector<Turn> &turns) -> Vector<Whole> {
  Vector<Whole> nexts(turns.size(), NONE);
  Vector<Whole> latest;
  for (Whole row = turns.size(); row > 0; --row) {
    const Whole number = turns[row - 1].number;
    if (number >= latest.size()) latest.resize(number + 1, NONE);
    nexts[row - 1] = latest[number];
    latest[number] = row - 1;
  }
  return nexts;
}

}  // namespace

void SOUND::TIMELINE::turned(
  Vector<Sent> &run, const Stock &stock, const Sending &where,
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole grain) {
  const Whole carried = INVENTORY::length(stock);
  const Vector<Whole> nexts = ::ahead(stock.turns);
  for (Whole row = 0; row < stock.turns.size(); ++row) {
    const Turn &move = stock.turns[row];
    const Whole next = nexts[row];
    const Flag held = next == NONE || move.shape == SHAPE::HOLD || grain == 0;
    const Whole ends = held ? move.at + 1 : stock.turns[next].at;
    for (Whole at = move.at; at < ends; at += held ? 1 : grain)
      stamp(
        run, where, span, carried,
        {.kind = KIND::CONTROL,
         .at = at,
         .number = move.number,
         .value = held ? move.value : valued(move, stock.turns[next], at)});
  }
}

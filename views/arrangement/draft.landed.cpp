// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../score.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "draft.internal.hpp"

namespace {
using namespace SOUND;

auto stated(Whole kind, Whole opens, Whole closes) -> Whole {
  if (kind != KIND::NOTES) return closes - opens;
  const Vector<Tempo> &tempos = TRANSPORT::held().tempos;
  return SCORE::pulsed(closes, tempos) - SCORE::pulsed(opens, tempos);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::DRAFT::landed(
  const GUI::SAC::LADDER::Rung &step,
  const GUI::SAC::STROKE::Gesture &gesture) {
  const Row seat = seated(gesture);
  if (!gesture.dragged || seat.track == NONE) return;
  const Run run = pulled(gesture, step.span, snapped());
  if (
    run.closes <= run.opens ||
    TIMELINE::crossed(seat.track, seat.lane, run.opens, run.closes, NONE))
    return;
  const Whole kind = TIMELINE::held().tracks[seat.track].lanes[seat.lane].kind;
  const Whole stock = INVENTORY::add(
    std::format("{}{}", KIND::spoken(kind), INVENTORY::held().stocks.size()),
    kind);
  if (stock == NONE) return;
  INVENTORY::length(stock, ::stated(kind, run.opens, run.closes));
  TIMELINE::place(
    INVENTORY::held(), seat.track, seat.lane,
    {.stock = stock,
     .at = run.opens,
     .from = 0,
     .frames = run.closes - run.opens});
}

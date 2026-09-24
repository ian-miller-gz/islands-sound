// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MODE = "draft";
constexpr STRING::Hot PICKED = "pick";
constexpr Float DEEP = 1.0f;
constexpr Float ORIGIN = 0.0f;

struct Run {
  Whole opens = 0, closes = 0;
};

auto pulled(const GUI::SAC::STROKE::Gesture &gesture, Whole step, Flag settle)
  -> ::Run {
  const Float west =
    std::max(::ORIGIN, std::min(gesture.from.at.x, gesture.to.at.x));
  const Float east =
    std::max(::ORIGIN, std::max(gesture.from.at.x, gesture.to.at.x));
  const Float grain = Float(VIEWS::ARRANGEMENT::GRAIN);
  const Whole opens = Whole(west * grain);
  const Whole closes = Whole(east * grain);
  if (step == 0 || !settle) return {opens, closes};
  return {opens / step * step, (closes + step - 1) / step * step};
}

auto pencilled(const GUI::SAC::STROKE::Gesture &gesture) -> Flag {
  return !VIEWS::ARRANGEMENT::gutter(gesture.from.at.x) &&
         !VIEWS::ARRANGEMENT::shelved(gesture.from.at.y) &&
         VIEWS::ARRANGEMENT::boxed(gesture.from.at.x, gesture.from.at.y) ==
           NONE;
}

auto seated(const GUI::SAC::STROKE::Gesture &gesture)
  -> VIEWS::ARRANGEMENT::Row {
  const VIEWS::ARRANGEMENT::Row seat =
    VIEWS::ARRANGEMENT::row(gesture.from.at.y);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    seat.track >= tracks.size() || seat.lane >= tracks[seat.track].lanes.size())
    return {};
  return seat;
}

void landed(
  const GUI::SAC::LADDER::Rung &step,
  const GUI::SAC::STROKE::Gesture &gesture) {
  const VIEWS::ARRANGEMENT::Row seat = ::seated(gesture);
  if (!gesture.dragged || seat.track == NONE) return;
  const ::Run run = ::pulled(gesture, step.span, VIEWS::ARRANGEMENT::snapped());
  if (
    run.closes <= run.opens ||
    TIMELINE::crossed(seat.track, seat.lane, run.opens, run.closes, NONE))
    return;
  const Whole kind = TIMELINE::held().tracks[seat.track].lanes[seat.lane].kind;
  const Whole stock = INVENTORY::add(
    std::format("{}{}", KIND::spoken(kind), INVENTORY::held().stocks.size()),
    kind);
  if (stock == NONE) return;
  TIMELINE::place(
    INVENTORY::held(), seat.track, seat.lane,
    {.stock = stock,
     .at = run.opens,
     .from = 0,
     .frames = run.closes - run.opens});
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::draft() {
  const GUI::Handle page = VIEWS::document();
  const GUI::SAC::LADDER::Rung step = rung();
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(page);
  GUI::SAC::STROKE::draw(
    page, {board().c_str(),
           drawing(gesture) ? ::MODE : ::PICKED,
           {across(step.span), ::DEEP},
           GUI::SAC::STROKE::Mode::BOARD});
  if (gesture.closed && drawing(gesture) && ::pencilled(gesture))
    ::landed(step, gesture);
}

auto SOUND::VIEWS::ARRANGEMENT::drafting() -> Draft {
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(VIEWS::document());
  if (!gesture.standing || !drawing(gesture) || !::pencilled(gesture))
    return {};
  const Row seat = ::seated(gesture);
  if (seat.track == NONE) return {};
  const ::Run run = ::pulled(gesture, rung().span, false);
  return {seat.track, seat.lane,
          seat.band,  TIMELINE::held().tracks[seat.track].lanes[seat.lane].kind,
          run.opens,  run.closes > run.opens ? run.closes - run.opens : 0};
}

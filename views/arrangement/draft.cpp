// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../timeline.hpp"
#include "draft.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MODE = "draft";
constexpr STRING::Hot PICKED = "pick";
constexpr Float DEEP = 1.0f;
constexpr Float ORIGIN = 0.0f;

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::DRAFT::pulled(
  const GUI::SAC::STROKE::Gesture &gesture, Whole step, Flag settle) -> Run {
  const Float west =
    std::max(::ORIGIN, std::min(gesture.from.at.x, gesture.to.at.x));
  const Float east =
    std::max(::ORIGIN, std::max(gesture.from.at.x, gesture.to.at.x));
  const Float grain = Float(GRAIN);
  const Whole opens = Whole(west * grain);
  const Whole closes = Whole(east * grain);
  if (step == 0 || !settle) return {opens, closes};
  return {opens / step * step, (closes + step - 1) / step * step};
}

auto SOUND::VIEWS::ARRANGEMENT::DRAFT::pencilled(
  const GUI::SAC::STROKE::Gesture &gesture) -> Flag {
  return !gutter(gesture.from.at.x) && !shelved(gesture.from.at.y) &&
         boxed(gesture.from.at.x, gesture.from.at.y) == NONE;
}

auto SOUND::VIEWS::ARRANGEMENT::DRAFT::seated(
  const GUI::SAC::STROKE::Gesture &gesture) -> Row {
  const Row seat = row(gesture.from.at.y);
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    seat.track >= tracks.size() || seat.lane >= tracks[seat.track].lanes.size())
    return {};
  return seat;
}

void SOUND::VIEWS::ARRANGEMENT::draft() {
  const GUI::SAC::LADDER::Rung step = rung();
  const GUI::SAC::STROKE::Gesture gesture = VIEWS::GET::gesture();
  VIEWS::draw(
    {board().c_str(),
     drawing(gesture) ? ::MODE : ::PICKED,
     {across(step.span), ::DEEP},
     GUI::SAC::STROKE::Mode::BOARD});
  if (gesture.closed && drawing(gesture) && DRAFT::pencilled(gesture))
    DRAFT::landed(step, gesture);
}

auto SOUND::VIEWS::ARRANGEMENT::drafting() -> Draft {
  const GUI::SAC::STROKE::Gesture gesture = VIEWS::GET::gesture();
  if (!gesture.standing || !drawing(gesture) || !DRAFT::pencilled(gesture))
    return {};
  const Row seat = DRAFT::seated(gesture);
  if (seat.track == NONE) return {};
  const DRAFT::Run run = DRAFT::pulled(gesture, rung().span, false);
  return {seat.track, seat.lane,
          seat.band,  TIMELINE::held().tracks[seat.track].lanes[seat.lane].kind,
          run.opens,  run.closes > run.opens ? run.closes - run.opens : 0};
}

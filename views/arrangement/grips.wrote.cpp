// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/stroke.hpp>

#include "../../history.hpp"
#include "../../timeline.hpp"
#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

auto moved(const Box &cut, const Box &was) -> Placement {
  return {
    .track = cut.track,
    .lane = cut.lane,
    .placement = cut.placement,
    .span = VIEWS::ARRANGEMENT::clip(cut, was),
    .was = VIEWS::ARRANGEMENT::clip(was, was)};
}

void spanned(const Vector<Carry> &carried) {
  Vector<Placement> shaped;
  for (const Carry &carry : carried) {
    const Box was = VIEWS::ARRANGEMENT::boxes()[carry.row];
    if (TIMELINE::span(
          carry.cut.track, carry.cut.lane, carry.cut.placement,
          VIEWS::ARRANGEMENT::clip(carry.cut, was)))
      shaped.push_back(::moved(carry.cut, was));
  }
  if (!shaped.empty())
    HISTORY::record(
      {.act = VIEWS::ARRANGEMENT::MOVED,
       .verb = Edit::CHANGED,
       .placements = shaped});
}

void shaped(const Box &cut, const Box &was) {
  if (!TIMELINE::span(
        cut.track, cut.lane, cut.placement, VIEWS::ARRANGEMENT::clip(cut, was)))
    return;
  HISTORY::record(
    {.act = VIEWS::ARRANGEMENT::SHAPED,
     .verb = Edit::CHANGED,
     .placements = {::moved(cut, was)}});
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::grips() {
  const GUI::SAC::STROKE::Gesture gesture = VIEWS::GET::gesture();
  if (!gesture.closed || !gesture.dragged) return;
  const Claim claim = claimed(gesture);
  if (claim.row == NONE) return;
  HISTORY::begin();
  if (claim.end != NONE) {
    ::shaped(landing(claim, gesture, snapped()), boxes()[claim.row]);
    return HISTORY::end();
  }
  const Vector<Carry> carried = travelled(claim, gesture, snapped());
  if (carried.empty()) return HISTORY::end();
  const Box was = boxes()[claim.row];
  const Box &now = carried.front().cut;
  if (now.track == was.track && now.lane == was.lane && !crowded(carried))
    ::spanned(carried);
  else
    relaid(carried);
  HISTORY::end();
}

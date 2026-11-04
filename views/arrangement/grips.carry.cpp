// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

auto clamped(
  const Vector<Box> &laid, const Vector<Whole> &group, Integer travel)
  -> Integer {
  for (const Whole row : group)
    travel = std::max(travel, -Integer(laid[row].at));
  return travel;
}

auto followed(const Box &mate, const Box &lead, Integer travel) -> Box {
  Box cut = mate;
  cut.at = Whole(Integer(cut.at) + travel);
  cut.track = lead.track;
  cut.lane = lead.lane;
  cut.band = lead.band;
  return cut;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::clip(const Box &cut, const Box &was)
  -> SOUND::ARRANGEMENT::TRACK::LANE::Clip {
  const Vector<SOUND::ARRANGEMENT::TRACK::LANE::Clip> spans =
    TIMELINE::spans(was.track, was.lane);
  SOUND::ARRANGEMENT::TRACK::LANE::Clip span = {.stock = cut.stock};
  if (was.placement < spans.size()) span = spans[was.placement];
  span.at = cut.at;
  span.from = cut.from;
  span.frames = cut.frames;
  return span;
}

auto SOUND::VIEWS::ARRANGEMENT::travelled(
  const Claim &claim, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Vector<Carry> {
  const Vector<Box> &laid = boxes();
  const Box lead = landing(claim, gesture, settle);
  const Vector<Whole> group = mates(claim.row);
  const Integer travel = ::clamped(
    laid, group, Integer(lead.at) - Integer(laid[claim.row].at));
  Vector<Carry> moved;
  for (const Whole row : group)
    moved.push_back({row, ::followed(laid[row], lead, travel)});
  std::sort(
    moved.begin(), moved.end(), [travel](const Carry &one, const Carry &two) {
      return travel > 0 ? one.cut.at > two.cut.at : one.cut.at < two.cut.at;
    });
  return moved;
}

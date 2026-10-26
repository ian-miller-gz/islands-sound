// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

auto handed(const GUI::SAC::STROKE::Gesture &gesture) -> Integer {
  return Integer(VIEWS::ARRANGEMENT::framed(gesture.to.at.x)) -
         Integer(VIEWS::ARRANGEMENT::framed(gesture.from.at.x));
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

auto SOUND::VIEWS::ARRANGEMENT::travelled(
  const Claim &claim, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Vector<Carry> {
  const Vector<Box> &laid = boxes();
  const Box lead = landing(claim, gesture, settle);
  const Vector<Whole> group = mates(claim.row);
  const Travel room = roomed(
    group, lead, Integer(lead.at) - Integer(laid[claim.row].at),
    ::handed(gesture));
  if (!room.stood) return {};
  const Integer travel = room.by;
  Vector<Carry> moved;
  for (const Whole row : group)
    moved.push_back({row, ::followed(laid[row], lead, travel)});
  std::sort(
    moved.begin(), moved.end(), [travel](const Carry &one, const Carry &two) {
      return travel > 0 ? one.cut.at > two.cut.at : one.cut.at < two.cut.at;
    });
  return moved;
}

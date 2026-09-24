// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>

#include "../../timeline.hpp"
#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
using VIEWS::ARRANGEMENT::Box;

auto strangers(const Box &cut, const Vector<Whole> &group) -> Vector<Clip> {
  const auto &clips = TIMELINE::held().tracks[cut.track].lanes[cut.lane].clips;
  const Vector<Box> &laid = VIEWS::ARRANGEMENT::boxes();
  Vector<Clip> others;
  for (Whole index = 0; index < clips.size(); ++index) {
    const Flag mate = std::any_of(group.begin(), group.end(), [&](Whole row) {
      return laid[row].track == cut.track && laid[row].lane == cut.lane &&
             laid[row].placement == index;
    });
    if (!mate) others.push_back(clips[index]);
  }
  return others;
}

auto crossed(const Box &member, Integer travel, const Vector<Clip> &others)
  -> Flag {
  if (travel < 0 && Whole(-travel) > member.at) return true;
  const Whole opens = Whole(Integer(member.at) + travel);
  const Whole closes = opens + member.frames;
  return std::any_of(others.begin(), others.end(), [&](const Clip &one) {
    return one.at < closes && opens < one.at + one.frames;
  });
}

auto flush(const Box &member, const Clip &other, Integer hand, Flag beyond)
  -> Integer {
  const Flag east = (hand >= 0) != beyond;
  return east ? Integer(other.at) - Integer(member.at + member.frames)
              : Integer(other.at + other.frames) - Integer(member.at);
}

auto nearer(Integer hand) {
  return [hand](Integer one, Integer two) {
    return std::abs(one - hand) < std::abs(two - hand);
  };
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::roomed(
  const Vector<Whole> &group, const Box &lead, Integer asked,
  Integer hand) -> Travel {
  const Vector<Box> &laid = boxes();
  const Vector<Clip> others = ::strangers(lead, group);
  const auto clear = [&](Integer travel) {
    return std::none_of(group.begin(), group.end(), [&](Whole row) {
      return ::crossed(laid[row], travel, others);
    });
  };
  if (clear(asked)) return {true, asked};
  for (const Flag beyond : {false, true}) {
    Vector<Integer> travels;
    for (const Whole row : group)
      for (const Clip &other : others)
        travels.push_back(::flush(laid[row], other, hand, beyond));
    std::sort(travels.begin(), travels.end(), ::nearer(hand));
    for (const Integer travel : travels)
      if (clear(travel)) return {true, travel};
  }
  return {};
}

auto SOUND::VIEWS::ARRANGEMENT::walled(const Claim &claim, Whole landed)
  -> Whole {
  const Box &box = boxes()[claim.row];
  Whole held = landed;
  for (const Clip &other : ::strangers(box, {claim.row})) {
    if (claim.end == 1 && other.at > box.at) held = std::min(held, other.at);
    if (claim.end == 0 && other.at < box.at)
      held = std::max(held, other.at + other.frames);
  }
  return held;
}

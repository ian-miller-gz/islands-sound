// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

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

auto crossed(const Box &cut, const Vector<Clip> &others) -> Flag {
  const Whole closes = cut.at + cut.frames;
  return std::any_of(others.begin(), others.end(), [&](const Clip &one) {
    return one.at < closes && cut.at < one.at + one.frames;
  });
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::crowded(const Vector<Carry> &carried)
  -> Flag {
  Vector<Whole> group;
  for (const Carry &carry : carried) group.push_back(carry.row);
  return std::any_of(carried.begin(), carried.end(), [&](const Carry &carry) {
    return ::crossed(carry.cut, ::strangers(carry.cut, group));
  });
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

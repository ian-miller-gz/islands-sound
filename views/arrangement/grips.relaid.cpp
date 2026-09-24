// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

auto clip(const Box &cut) -> ARRANGEMENT::TRACK::LANE::Clip {
  return {
    .stock = cut.stock, .at = cut.at, .from = cut.from, .frames = cut.frames};
}

auto lifted(const Vector<Carry> &moved) -> Vector<Placement> {
  const Vector<Box> &laid = VIEWS::ARRANGEMENT::boxes();
  Vector<Placement> lifted;
  for (const Carry &carry : moved) {
    const Box &was = laid[carry.row];
    lifted.push_back({was.track, was.lane, was.placement, ::clip(was)});
  }
  std::sort(lifted.begin(), lifted.end(), [](const auto &one, const auto &two) {
    return one.placement < two.placement;
  });
  for (Whole back = lifted.size(); back > 0; --back)
    TIMELINE::lift(
      lifted[back - 1].track, lifted[back - 1].lane,
      lifted[back - 1].placement);
  return lifted;
}

auto placed(const Vector<Carry> &moved) -> Vector<Placement> {
  Vector<Placement> placed;
  for (const Carry &carry : moved) {
    const Whole index = TIMELINE::place(
      INVENTORY::held(), carry.cut.track, carry.cut.lane, ::clip(carry.cut));
    if (index != NONE)
      placed.push_back(
        {carry.cut.track, carry.cut.lane, index, ::clip(carry.cut)});
  }
  return placed;
}

auto phrased(const Vector<Carry> &moved) -> String {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  const Whole track = moved.front().cut.track;
  return std::format(
    "{} {} clip{} to {}", VIEWS::ARRANGEMENT::MOVED, moved.size(),
    moved.size() == 1 ? "" : "s",
    track < tracks.size() ? tracks[track].name : String());
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::relaid(Vector<Carry> moved) {
  std::sort(moved.begin(), moved.end(), [](const Carry &one, const Carry &two) {
    return one.cut.at < two.cut.at;
  });
  HISTORY::begin();
  HISTORY::said(::phrased(moved));
  HISTORY::record(
    {.act = MOVED, .verb = Edit::REMOVED, .placements = ::lifted(moved)});
  const Vector<Placement> placed = ::placed(moved);
  HISTORY::record({.act = MOVED, .placements = placed});
  HISTORY::end();
  Vector<Whole> rows;
  for (const Placement &one : placed)
    rows.push_back(rowed(one.track, one.lane, one.placement));
  chosen(rows);
}

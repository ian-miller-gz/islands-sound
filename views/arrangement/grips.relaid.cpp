// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../commands.hpp"
#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

auto standing(const Vector<Carry> &moved) -> Vector<Placement> {
  const Vector<Box> &laid = VIEWS::ARRANGEMENT::boxes();
  Vector<Placement> stood;
  for (const Carry &carry : moved) {
    const Box &was = laid[carry.row];
    stood.push_back(
      {was.track, was.lane, was.placement, VIEWS::ARRANGEMENT::clip(was, was)});
  }
  return stood;
}

auto joined(Vector<Placement> into, const Vector<Placement> &from)
  -> Vector<Placement> {
  into.insert(into.end(), from.begin(), from.end());
  return into;
}

auto lifted(Vector<Placement> gone) -> Vector<Placement> {
  std::sort(gone.begin(), gone.end(), [](const auto &one, const auto &two) {
    return one.placement < two.placement;
  });
  for (Whole back = gone.size(); back > 0; --back)
    TIMELINE::lift(
      gone[back - 1].track, gone[back - 1].lane, gone[back - 1].placement);
  return gone;
}

auto landed(const Vector<Carry> &moved) -> Vector<Placement> {
  const Vector<Box> &laid = VIEWS::ARRANGEMENT::boxes();
  Vector<Placement> landing;
  for (const Carry &carry : moved)
    landing.push_back(
      {carry.cut.track, carry.cut.lane, NONE,
       VIEWS::ARRANGEMENT::clip(carry.cut, laid[carry.row])});
  return landing;
}

auto placed(Vector<Placement> landing) -> Vector<Placement> {
  Vector<Placement> placed;
  for (Placement &one : landing) {
    one.placement =
      TIMELINE::place(INVENTORY::held(), one.track, one.lane, one.span);
    if (one.placement != NONE) placed.push_back(one);
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
  const Vector<Placement> landing = ::landed(moved);
  const Vector<Placement> stood = ::standing(moved);
  const COMMANDS::SPLIT::Yield yield = COMMANDS::SPLIT::yielded(landing, stood);
  HISTORY::record(
    {.act = MOVED,
     .verb = Edit::REMOVED,
     .placements = ::lifted(::joined(stood, yield.lifted))});
  const Vector<Placement> kept = ::placed(yield.kept);
  const Vector<Placement> placed = ::placed(landing);
  HISTORY::record({.act = MOVED, .placements = ::joined(kept, placed)});
  HISTORY::end();
  Vector<Whole> rows;
  for (const Placement &one : placed)
    rows.push_back(rowed(one.track, one.lane, one.placement));
  chosen(rows);
}

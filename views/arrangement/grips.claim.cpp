// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

auto grip() -> Whole {
  return VIEWS::ARRANGEMENT::framed(
    VIEWS::ARRANGEMENT::GRIP / VIEWS::ARRANGEMENT::scaled());
}

auto ended(const Box &box, Whole at) -> Whole {
  if (!VIEWS::ARRANGEMENT::gripped(box.frames)) return NONE;
  const Whole opens = at < box.at ? box.at - at : at - box.at;
  const Whole east = box.at + box.frames;
  const Whole closes = at < east ? east - at : at - east;
  if (opens > ::grip() && closes > ::grip()) return NONE;
  return closes <= opens ? 1 : 0;
}

auto runged(Whole at) -> Whole {
  const Whole step = VIEWS::ARRANGEMENT::rung().span;
  return step == 0 ? at : (at + step / 2) / step * step;
}

auto settled(const Box &box, Whole at) -> Whole {
  const Whole step = VIEWS::ARRANGEMENT::rung().span;
  Whole best = ::runged(at);
  Whole closest = step + 1;
  for (const Whole mark : VIEWS::ARRANGEMENT::marks(box)) {
    const Whole notch = box.at + mark;
    const Whole gap = notch < at ? at - notch : notch - at;
    if (gap <= step && gap < closest) {
      closest = gap;
      best = notch;
    }
  }
  return best;
}

auto carried(Box box, Whole press, Whole to, Flag settle) -> Box {
  const Whole at = to >= press           ? box.at + (to - press)
                   : box.at > press - to ? box.at - (press - to)
                                         : 0;
  box.at = settle ? ::runged(at) : at;
  return box;
}

auto shaped(Box box, Flag east, Whole landed) -> Box {
  if (east) {
    if (landed <= box.at) return box;
    box.frames = landed - box.at;
    return box;
  }
  if (landed >= box.at + box.frames) return box;
  const Whole moved = VIEWS::ARRANGEMENT::stocked(
    box, landed < box.at ? box.at - landed : landed - box.at);
  box.from = landed < box.at ? (box.from > moved ? box.from - moved : 0)
                             : box.from + moved;
  box.frames = box.at + box.frames - landed;
  box.at = landed;
  return box;
}

auto seated(const Box &box, Float down) -> VIEWS::ARRANGEMENT::Row {
  const VIEWS::ARRANGEMENT::Row own = {box.track, box.lane, box.band};
  const VIEWS::ARRANGEMENT::Row seat = VIEWS::ARRANGEMENT::row(down);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    seat.track == NONE || seat.track >= tracks.size() ||
    seat.lane >= tracks[seat.track].lanes.size())
    return own;
  if (tracks[seat.track].lanes[seat.lane].kind != box.kind) return {};
  return seat;
}

auto moved(
  const Box &box, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Box {
  const VIEWS::ARRANGEMENT::Row seat = ::seated(box, gesture.to.at.y);
  if (seat.track == NONE) return box;
  Box cut = ::carried(
    box, VIEWS::ARRANGEMENT::framed(gesture.from.at.x),
    VIEWS::ARRANGEMENT::framed(gesture.to.at.x), settle);
  cut.track = seat.track;
  cut.lane = seat.lane;
  cut.band = seat.band;
  return cut;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::framed(Float across) -> Whole {
  return across <= 0.0f ? 0 : Whole(across * Float(GRAIN));
}

auto SOUND::VIEWS::ARRANGEMENT::claimed(
  const GUI::SAC::STROKE::Gesture &gesture) -> Claim {
  const Whole row = boxed(gesture.from.at.x, gesture.from.at.y);
  const Vector<Box> &laid = boxes();
  if (row == NONE || row >= laid.size()) return {};
  return {row, ::ended(laid[row], framed(gesture.from.at.x))};
}

auto SOUND::VIEWS::ARRANGEMENT::landing(
  const Claim &claim, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Box {
  const Box box = boxes()[claim.row];
  if (claim.end == NONE) return ::moved(box, gesture, settle);
  const Whole to = framed(gesture.to.at.x);
  const Flag east = claim.end == 1;
  const Whole aimed = !settle ? to : east ? ::settled(box, to) : ::runged(to);
  const Box cut = ::shaped(box, east, walled(claim, aimed));
  return cut.frames == 0 ? box : cut;
}

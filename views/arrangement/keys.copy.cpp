// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"
#include "keys.internal.hpp"

namespace {
using namespace SOUND;

struct Copied {
  Flag full = false;
  String track, lane;
  ARRANGEMENT::TRACK::LANE::Clip span;
};

Copied carried;

constexpr Whole HALVED = 2;

auto runged(Whole at) -> Whole {
  const Whole step = VIEWS::ARRANGEMENT::rung().span;
  if (!VIEWS::ARRANGEMENT::snapped() || step == 0) return at;
  return (at + step / ::HALVED) / step * step;
}

auto landing(Whole frames) -> Whole {
  const Whole at = TRANSPORT::marker().position;
  const VIEWS::ARRANGEMENT::Seen view = VIEWS::ARRANGEMENT::seen();
  if (view.closes <= view.opens) return at;
  if (at >= view.opens && at < view.closes) return at;
  const Whole centre = (view.opens + view.closes) / ::HALVED;
  const Whole half = frames / ::HALVED;
  return ::runged(centre > half ? centre - half : 0);
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::chosen(GUI::Handle page) -> Whole {
  const Whole chose = GUI::SAC::SEAT::selected(page, board().c_str());
  return chose < boxes().size() ? chose : NONE;
}

void SOUND::VIEWS::ARRANGEMENT::copy(GUI::Handle page) {
  const Whole chose = chosen(page);
  if (chose == NONE) return;
  const Box &held = boxes()[chose];
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    held.track >= tracks.size() || held.lane >= tracks[held.track].lanes.size())
    return;
  const SOUND::ARRANGEMENT::TRACK::Lane &lane =
    tracks[held.track].lanes[held.lane];
  if (held.placement >= lane.clips.size()) return;
  ::carried = {
    true, tracks[held.track].name, lane.name, lane.clips[held.placement]};
}

void SOUND::VIEWS::ARRANGEMENT::paste(GUI::Handle) {
  if (!::carried.full) return;
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track) {
    if (tracks[track].name != ::carried.track) continue;
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane) {
      if (tracks[track].lanes[lane].name != ::carried.lane) continue;
      SOUND::ARRANGEMENT::TRACK::LANE::Clip span = ::carried.span;
      span.at = ::landing(span.frames);
      const Whole index = TIMELINE::place(INVENTORY::held(), track, lane, span);
      if (index == NONE) return;
      HISTORY::record(
        {.act = "clip",
         .placements = {
           {.track = track, .lane = lane, .placement = index, .span = span}}});
      return;
    }
  }
}

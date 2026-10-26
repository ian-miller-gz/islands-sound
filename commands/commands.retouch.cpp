// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../history.hpp"
#include "../timeline.hpp"
#include "commands.retouch.hpp"

using namespace SOUND;

auto SOUND::COMMANDS::retouch(
  Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &now, STRING::Hot act) -> Flag {
  const Vector<ARRANGEMENT::TRACK::LANE::Clip> spans =
    TIMELINE::spans(track, lane);
  if (placement >= spans.size()) return false;
  if (!TIMELINE::span(track, lane, placement, now)) return false;
  HISTORY::record(
    {.act = act,
     .verb = Edit::CHANGED,
     .placements = {
       {.track = track,
        .lane = lane,
        .placement = placement,
        .span = now,
        .was = spans[placement]}}});
  return true;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ATTENDED = "tracks";

}  // namespace

auto SOUND::VIEWS::ARRANGER::paged(Whole kind) -> Whole {
  for (Whole page = 0; page < COUNT; ++page)
    if (kind != NONE && PAGES[page].kind == kind) return page;
  return NONE;
}

auto SOUND::VIEWS::ARRANGER::offered() -> LANE::Mark {
  const LANE::Mark mark = LANE::marked();
  if (paged(mark.kind) != NONE) return mark;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  const Whole track = VIEWS::cursor(::ATTENDED);
  if (track >= tracks.size()) return {};
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes = tracks[track].lanes;
  for (Whole lane = 0; lane < lanes.size(); ++lane)
    if (paged(lanes[lane].kind) != NONE) return {track, lane, lanes[lane].kind};
  return {};
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ATTENDED = "tracks";
constexpr Whole OFFERED[] = {
  KIND::NOTES, KIND::AUDIO, KIND::CONTROL, KIND::PROGRAM};

auto first(const ARRANGEMENT::Track &track, Whole kind) -> Whole {
  for (Whole lane = 0; lane < track.lanes.size(); ++lane)
    if (track.lanes[lane].kind == kind) return lane;
  return NONE;
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::offered() -> LANE::Mark {
  const LANE::Mark mark = LANE::marked();
  if (LANE::editable(mark.kind)) return mark;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  const Whole track = VIEWS::cursor(::ATTENDED);
  if (track >= tracks.size()) return {};
  for (const Whole kind : ::OFFERED) {
    const Whole lane = ::first(tracks[track], kind);
    if (lane != NONE && LANE::editable(kind)) return {track, lane, kind};
  }
  return {};
}

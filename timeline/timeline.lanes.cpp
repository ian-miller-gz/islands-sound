// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

auto SOUND::TIMELINE::spans(Whole track, Whole lane)
  -> Vector<ARRANGEMENT::TRACK::LANE::Clip> {
  if (track >= held().tracks.size()) return {};
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes = held().tracks[track].lanes;
  if (lane >= lanes.size()) return {};
  return lanes[lane].clips;
}

auto SOUND::TIMELINE::covered(Whole track, Whole lane, Whole at)
  -> ARRANGEMENT::TRACK::LANE::Clip {
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : spans(track, lane))
    if (at >= span.at && at - span.at < span.frames) return span;
  return {};
}

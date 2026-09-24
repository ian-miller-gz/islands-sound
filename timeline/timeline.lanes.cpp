// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

auto SOUND::TIMELINE::spans(Whole track, Whole kind)
  -> Vector<ARRANGEMENT::TRACK::LANE::Clip> {
  Vector<ARRANGEMENT::TRACK::LANE::Clip> placed;
  if (track >= held().tracks.size()) return placed;
  for (const ARRANGEMENT::TRACK::Lane &lane : held().tracks[track].lanes) {
    if (lane.kind != kind) continue;
    placed.insert(placed.end(), lane.clips.begin(), lane.clips.end());
  }
  return placed;
}

auto SOUND::TIMELINE::covered(Whole track, Whole kind, Whole at)
  -> ARRANGEMENT::TRACK::LANE::Clip {
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : spans(track, kind))
    if (at >= span.at && at - span.at < span.frames) return span;
  return {};
}

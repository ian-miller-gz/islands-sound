// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

auto SOUND::TIMELINE::track(Whole at, const ARRANGEMENT::Track &row) -> Flag {
  Vector<ARRANGEMENT::Track> &tracks = standing().tracks;
  if (at > tracks.size()) return false;
  tracks.insert(tracks.begin() + static_cast<Integer>(at), row);
  return true;
}

auto SOUND::TIMELINE::lane(
  Whole track, Whole at, const ARRANGEMENT::TRACK::Lane &row) -> Flag {
  if (track >= standing().tracks.size()) return false;
  Vector<ARRANGEMENT::TRACK::Lane> &lanes = standing().tracks[track].lanes;
  if (at > lanes.size()) return false;
  lanes.insert(lanes.begin() + static_cast<Integer>(at), row);
  return true;
}

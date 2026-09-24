// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "timeline.internal.hpp"

auto SOUND::TIMELINE::ended() -> Whole {
  Whole reach = 0;
  for (const ARRANGEMENT::Track &track : held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        reach = std::max(reach, span.at + span.frames);
  return reach;
}

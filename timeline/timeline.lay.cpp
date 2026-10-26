// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

void copied(
  Vector<TIMELINE::Laid> &run, const Take &take, Whole track,
  const String &root, Whole lane, Whole at, Whole reads, Whole length) {
  TIMELINE::Laid stretch = {
    .track = track, .root = root, .lane = lane, .at = at, .lanes = {}};
  for (const Vector<Float> &channel : take.lanes)
    stretch.lanes.push_back(
      {channel.begin() + reads, channel.begin() + reads + length});
  run.push_back(std::move(stretch));
}

void stretched(
  Vector<TIMELINE::Laid> &run, const Take &take, Whole track,
  const String &root, Whole lane, const ARRANGEMENT::TRACK::LANE::Clip &span,
  Whole from, Whole until) {
  const Whole carried = take.lanes.empty() ? 0 : take.lanes[0].size();
  if (carried == 0) return;
  const Whole ends = std::min(span.at + span.frames, until);
  for (Whole begins = std::max(span.at, from); begins < ends;) {
    const Whole reads = TIMELINE::wrapped(span.from, begins - span.at, carried);
    const Whole length = std::min(ends - begins, carried - reads);
    ::copied(run, take, track, root, lane, begins - from, reads, length);
    begins += length;
  }
}

auto sooner(const TIMELINE::Laid &one, const TIMELINE::Laid &two) -> Flag {
  return one.at < two.at;
}

}  // namespace

auto SOUND::TIMELINE::lay(
  const Inventory &pool, Whole from, Whole frames,
  const Vector<String> &claims) -> Vector<Laid> {
  Vector<Laid> run;
  const Whole until = from + frames;
  for (Whole index = 0; index < held().tracks.size(); ++index) {
    const ARRANGEMENT::Track &track = held().tracks[index];
    if (track.root.empty() || !claimed(claims, track.root)) continue;
    for (Whole lane = 0; lane < track.lanes.size(); ++lane) {
      const ARRANGEMENT::TRACK::Lane &stood = track.lanes[lane];
      if (stood.kind != KIND::AUDIO) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : stood.clips)
        if (span.stock < pool.stocks.size())
          ::stretched(
            run, pool.stocks[span.stock].take, index, track.root, lane, span,
            from, until);
    }
  }
  std::stable_sort(run.begin(), run.end(), ::sooner);
  return run;
}

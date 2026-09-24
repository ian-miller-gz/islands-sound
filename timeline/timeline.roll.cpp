// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../score.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

void sounded(
  Vector<TIMELINE::Sounding> &run, const Stock &stock, Whole track,
  const String &root, Whole lane, const ARRANGEMENT::TRACK::LANE::Clip &span,
  Whole from, Whole until, const Vector<Tempo> &tempos) {
  const Whole carried = INVENTORY::length(stock);
  const Whole ends = std::min(span.at + span.frames, until);
  const Flag turns = carried > 0 && SCORE::framed(carried, tempos) > 0;
  for (const Note &note : stock.score.notes)
    for (Whole elapsed = TIMELINE::opened(note.at, span.from, carried);;
         elapsed += carried) {
      const Whole at = span.at + SCORE::framed(elapsed, tempos);
      if (at >= ends) break;
      if (at >= from)
        run.push_back(
          {.track = track,
           .root = root,
           .lane = lane,
           .at = at,
           .pitch = note.pitch,
           .length = std::min(
             SCORE::framed(note.length, tempos), span.at + span.frames - at),
           .velocity = note.velocity});
      if (!turns) break;
    }
}

auto sooner(const TIMELINE::Sounding &one, const TIMELINE::Sounding &two)
  -> Flag {
  return one.at < two.at;
}

}  // namespace

auto SOUND::TIMELINE::roll(
  const Inventory &pool, Whole from, Whole frames, const Vector<Tempo> &tempos,
  const Vector<String> &claims) -> Vector<Sounding> {
  Vector<Sounding> run;
  const Whole until = from + frames;
  for (Whole index = 0; index < held().tracks.size(); ++index) {
    const ARRANGEMENT::Track &track = held().tracks[index];
    if (track.root.empty() || !claimed(claims, track.root)) continue;
    for (Whole lane = 0; lane < track.lanes.size(); ++lane) {
      const ARRANGEMENT::TRACK::Lane &stood = track.lanes[lane];
      if (stood.kind != KIND::NOTES) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : stood.clips)
        if (span.stock < pool.stocks.size())
          ::sounded(
            run, pool.stocks[span.stock], index, track.root, lane, span, from,
            until, tempos);
    }
  }
  std::stable_sort(run.begin(), run.end(), ::sooner);
  return run;
}

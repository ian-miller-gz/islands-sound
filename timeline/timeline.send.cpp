// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

void chosen(
  Vector<TIMELINE::Sent> &run, const Stock &stock,
  const TIMELINE::Sending &where, const ARRANGEMENT::TRACK::LANE::Clip &span) {
  const Whole carried = INVENTORY::length(stock);
  for (const Choice &choice : stock.choices)
    TIMELINE::stamp(
      run, where, span, carried,
      {.kind = KIND::PROGRAM, .at = choice.at, .number = choice.program});
}

auto sooner(const TIMELINE::Sent &one, const TIMELINE::Sent &two) -> Flag {
  return one.at < two.at;
}

}  // namespace

void SOUND::TIMELINE::stamp(
  Vector<Sent> &run, const Sending &where,
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole carried, const Sent &edge) {
  const Whole ends = std::min(span.at + span.frames, where.until);
  for (Whole elapsed = opened(edge.at, span.from, carried);;
       elapsed += carried) {
    const Whole at = span.at + elapsed;
    if (at >= ends) break;
    if (at >= where.from)
      run.push_back(
        {.track = where.track,
         .root = where.root,
         .lane = where.lane,
         .kind = edge.kind,
         .at = at,
         .number = edge.number,
         .value = edge.value});
    if (carried == 0) break;
  }
}

auto SOUND::TIMELINE::send(
  const Inventory &pool, Whole from, Whole frames, Whole grain,
  const Vector<String> &claims) -> Vector<Sent> {
  Vector<Sent> run;
  for (Whole index = 0; index < held().tracks.size(); ++index) {
    const ARRANGEMENT::Track &track = held().tracks[index];
    if (track.root.empty() || !claimed(claims, track.root)) continue;
    for (Whole lane = 0; lane < track.lanes.size(); ++lane) {
      const ARRANGEMENT::TRACK::Lane &stood = track.lanes[lane];
      if (stood.kind != KIND::CONTROL && stood.kind != KIND::PROGRAM) continue;
      const Sending where = {
        .track = index,
        .root = track.root,
        .lane = lane,
        .from = from,
        .until = from + frames};
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : stood.clips) {
        if (span.stock >= pool.stocks.size()) continue;
        const Stock &stock = pool.stocks[span.stock];
        if (stood.kind == KIND::CONTROL)
          turned(run, stock, where, span, grain);
        else
          ::chosen(run, stock, where, span);
      }
    }
  }
  std::stable_sort(run.begin(), run.end(), ::sooner);
  return run;
}

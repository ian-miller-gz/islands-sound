// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto bare(const ARRANGEMENT::TRACK::Lane &lane) -> Flag {
  return lane.kind == KIND::DATA && lane.clips.empty() &&
         lane.outtakes.empty() && lane.map.empty();
}

void shed(Graph &wiring, const String &root, Whole out) {
  Vector<Wire> kept;
  for (Wire &wire : wiring.wires) {
    if (wire.from == root && wire.out == out) continue;
    if (wire.from == root && wire.out > out) --wire.out;
    kept.push_back(wire);
  }
  wiring.wires = kept;
}

}  // namespace

void SOUND::SESSION::culled(Arrangement &tracks, Graph &wiring) {
  for (ARRANGEMENT::Track &track : tracks.tracks)
    for (Whole lane = track.lanes.size(); lane-- > 0;) {
      if (!::bare(track.lanes[lane])) continue;
      track.lanes.erase(track.lanes.begin() + lane);
      if (!track.root.empty()) ::shed(wiring, track.root, lane);
    }
}

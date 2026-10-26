// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../inventory.hpp"
#include "../kind.hpp"
#include "../plugin.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

auto wired(const String &root, Whole out, const String &node) -> Flag {
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == root && wire.out == out && wire.to == node) return true;
  return false;
}

auto standing(const Vector<Turn> &turns, Whole number, Whole at) -> Flag {
  for (const Turn &move : turns)
    if (move.number == number && move.at <= at) return true;
  return false;
}

auto turned(
  const ARRANGEMENT::TRACK::Lane &lane, Whole number, Whole at,
  Float &value) -> Flag {
  const Inventory &pool = INVENTORY::held();
  Flag found = false;
  Whole begun = 0;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips) {
    if (span.stock >= pool.stocks.size() || span.frames == 0) continue;
    if (span.at > at || (found && span.at < begun)) continue;
    const Whole reached = std::min(at, span.at + span.frames - 1);
    const Whole elapsed = span.from + (reached - span.at);
    const Vector<Turn> &turns = pool.stocks[span.stock].turns;
    if (!::standing(turns, number, elapsed)) continue;
    found = true;
    begun = span.at;
    value = TIMELINE::valued(turns, number, elapsed);
  }
  return found;
}

}  // namespace

auto SOUND::RENDER::mapped(Whole track, Whole lane) -> String {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size()) return {};
  const ARRANGEMENT::Track &stood = tracks[track];
  if (!stood.lanes[lane].map.empty()) return stood.lanes[lane].map;
  return stood.root.empty() ? String() : GRAPH::fed(stood.root, lane);
}

auto SOUND::RENDER::worded(Whole track, Whole lane, Whole number, Float value)
  -> Float {
  const String plugin = mapped(track, lane);
  AUDIO::PLUGIN::Control described;
  if (plugin.empty() || !GRAPH::described(plugin, number, described)) return value;
  return PLUGIN::scaled(described, value);
}

auto SOUND::RENDER::governed(const Address &address, Whole at)
  -> TIMELINE::Governed {
  const TIMELINE::Governed curved =
    TIMELINE::govern(INVENTORY::held(), address, at);
  if (curved.stated) return curved;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track) {
    if (tracks[track].root.empty()) continue;
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane) {
      if (tracks[track].lanes[lane].kind != KIND::CONTROL) continue;
      if (!::wired(tracks[track].root, lane, address.node)) continue;
      Float value = 0.0f;
      if (!::turned(tracks[track].lanes[lane], address.parameter, at, value))
        continue;
      return {
        .stated = true, .value = worded(track, lane, address.parameter, value)};
    }
  }
  return {};
}

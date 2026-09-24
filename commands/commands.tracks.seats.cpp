// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "../kind.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto laning(Whole track, Whole lane) -> Laning {
  const ARRANGEMENT::Track &held = TIMELINE::held().tracks[track];
  Laning kept = {.track = track, .lane = lane, .row = held.lanes[lane]};
  if (!GRAPH::rooted(held.root)) return kept;
  const Node &root = GRAPH::held().nodes[GRAPH::at(held.root)];
  if (lane < root.outs.size()) kept.port = root.outs[lane];
  kept.admits = GRAPH::admitted(held.root, lane) != NONE;
  return kept;
}

auto shorn(const String &root, Whole lane) -> Vector<Wire> {
  const String taking = GRAPH::recorder(root);
  const Whole in = GRAPH::admitted(root, lane);
  Vector<Wire> cut;
  for (const Wire &wire : GRAPH::held().wires)
    if (
      (wire.from == root && wire.out == lane) ||
      (wire.to == root && wire.in == in) ||
      (wire.to == taking && wire.in == lane))
      cut.push_back(wire);
  return cut;
}

}  // namespace

auto SOUND::COMMANDS::unseated(const String &node) -> Flag {
  Vector<String> gone;
  return unseated(node, gone);
}

auto SOUND::COMMANDS::unseated(const String &node, Vector<String> &gone)
  -> Flag {
  if (!GRAPH::unseat(node, gone)) return false;
  CONTROL::settle(gone);
  return true;
}

auto SOUND::COMMANDS::root(Whole track, const String &name) -> String {
  const Vector<Whole> kinds = TIMELINE::kinds(track);
  const String node = GRAPH::root(kinds, name);
  for (Whole lane = 0; lane < kinds.size(); ++lane)
    if (kinds[lane] == KIND::AUDIO) GRAPH::admit(node, lane, true);
  return node;
}

auto SOUND::COMMANDS::laned(Whole track, Whole kind) -> Whole {
  const Whole lane = TIMELINE::lane(track, kind);
  if (lane == NONE) return NONE;
  const String root = TIMELINE::held().tracks[track].root;
  const Whole out = GRAPH::grow(root, kind);
  if (out != NONE && kind == KIND::AUDIO) GRAPH::admit(root, out, true);
  HISTORY::record({.act = "lane", .lanes = {::laning(track, lane)}});
  return lane;
}

auto SOUND::COMMANDS::unlaned(Whole track, Whole lane) -> Flag {
  if (
    track >= TIMELINE::held().tracks.size() ||
    lane >= TIMELINE::held().tracks[track].lanes.size())
    return false;
  const String root = TIMELINE::held().tracks[track].root;
  const Flag rooted = !root.empty();
  if (
    rooted && (!GRAPH::rooted(root) ||
               lane >= GRAPH::held().nodes[GRAPH::at(root)].outs.size()))
    return false;
  Edit edit = {.act = "unlane", .verb = Edit::REMOVED};
  edit.graph.wires = rooted ? ::shorn(root, lane) : Vector<Wire>();
  edit.lanes = {::laning(track, lane)};
  if (!TIMELINE::unlane(track, lane)) return false;
  if (rooted) GRAPH::shed(root, lane);
  HISTORY::record(edit);
  return true;
}

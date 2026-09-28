// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../kind.hpp"
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

constexpr Whole LANED = 5;

constexpr Whole KINDS[] = {
  KIND::NOTES, KIND::AUDIO, KIND::LOGIC, KIND::CONTROL, KIND::PROGRAM};

auto stamped(const Graph &wiring, const String &word) -> String {
  for (Whole count = 1;; ++count) {
    const String said = count == 1 ? word : std::format("{}{}", word, count);
    Flag worn = false;
    for (const Node &node : wiring.nodes) worn = worn || node.name == said;
    if (!worn) return said;
  }
}

void seated(std::istringstream &line, Graph &wiring) {
  Node node;
  line >> node.seat;
  const String word = SESSION::rest(line);
  if (node.seat == Node::PLUGIN) node.plugin = word;
  node.name = ::stamped(wiring, word);
  wiring.nodes.push_back(node);
}

void aimed(const String &tag, std::istringstream &line, Graph &wiring) {
  Whole node = NONE, other = NONE;
  if (tag == "c") {
    line >> node >> other;
    if (node < wiring.nodes.size())
      wiring.nodes[node].claim = SESSION::named(wiring, other);
  } else if (tag == "q") {
    line >> node;
    if (node < wiring.nodes.size()) wiring.nodes[node].quiet = true;
  } else {
    Wire wire;
    line >> node >> wire.out >> other >> wire.in;
    wire.from = SESSION::named(wiring, node);
    wire.to = SESSION::named(wiring, other);
    if (!wire.from.empty() && !wire.to.empty()) wiring.wires.push_back(wire);
  }
}

auto owner(const Arrangement &tracks, const String &root)
  -> const ARRANGEMENT::Track * {
  for (const ARRANGEMENT::Track &track : tracks.tracks)
    if (track.root == root) return &track;
  return nullptr;
}

auto lane(const Arrangement &tracks, const String &root, Whole out) -> Whole {
  const ARRANGEMENT::Track *track = ::owner(tracks, root);
  if (track == nullptr || out >= std::size(::KINDS)) return NONE;
  for (Whole at = 0; at < track->lanes.size(); ++at)
    if (track->lanes[at].kind == ::KINDS[out]) return at;
  return NONE;
}

auto rooted(const Graph &wiring, const String &node) -> Flag {
  for (const Node &one : wiring.nodes)
    if (one.name == node) return one.seat == Node::ROOT;
  return false;
}

}  // namespace

auto SOUND::SESSION::named(const Graph &wiring, Whole row) -> String {
  return row < wiring.nodes.size() ? wiring.nodes[row].name : String();
}

void SOUND::SESSION::older(
  const String &text, Graph &wiring, Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "s") {
      ::seated(line, wiring);
    } else if (tag == "h") {
      if (!wiring.nodes.empty()) {
        Node &node = wiring.nodes.back();
        line >> node.across >> node.down;
      }
    } else if (tag == "c" || tag == "q" || tag == "e") {
      ::aimed(tag, line, wiring);
    } else {
      husked(held, at, husks);
    }
  }
}

void SOUND::SESSION::admitted(
  Whole schema, const Arrangement &tracks, Graph &wiring) {
  if (schema >= ADMITTED) return;
  for (Node &node : wiring.nodes) {
    if (node.seat != Node::ROOT) continue;
    for (const ARRANGEMENT::Track &track : tracks.tracks) {
      if (track.root != node.name) continue;
      for (Whole lane = 0; lane < track.lanes.size(); ++lane)
        if (track.lanes[lane].kind == KIND::AUDIO)
          node.ins.push_back({.kind = KIND::AUDIO, .lane = lane});
    }
  }
}

void SOUND::SESSION::relaid(
  Whole schema, const Arrangement &tracks, Graph &wiring) {
  if (schema >= LANED) return;
  for (Wire &wire : wiring.wires)
    if (::rooted(wiring, wire.from))
      wire.out = ::lane(tracks, wire.from, wire.out);
}

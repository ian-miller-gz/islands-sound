// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

constexpr Whole BUSSED = 2;

constexpr STRING::Hot CLOCKED = "output";
constexpr STRING::Hot MASTER = "master";

auto stamped(const Graph &wiring, const String &word) -> String {
  for (Whole count = 1;; ++count) {
    const String said = count == 1 ? word : std::format("{}{}", word, count);
    Flag worn = false;
    for (const Node &node : wiring.nodes) worn = worn || node.name == said;
    if (!worn) return said;
  }
}

void untargeted(Graph &wiring, Whole master) {
  Vector<String> gone;
  for (Whole at = master + 1; at < wiring.nodes.size(); ++at)
    if (wiring.nodes[at].seat == ::BUSSED)
      gone.push_back(wiring.nodes[at].name);
  for (const String &name : gone) {
    std::erase_if(
      wiring.nodes, [&](const Node &node) { return node.name == name; });
    std::erase_if(wiring.wires, [&](const Wire &wire) {
      return wire.from == name || wire.to == name;
    });
  }
}

void tracked(Arrangement &tracks, const String &root) {
  for (ARRANGEMENT::Track &track : tracks.tracks)
    if (track.root == root) {
      track.bus = true;
      return;
    }
  const ARRANGEMENT::TRACK::Lane audio = {
    .kind = KIND::AUDIO, .name = String(KIND::spoken(KIND::AUDIO))};
  tracks.tracks.push_back(
    {.name = String(::MASTER), .lanes = {audio}, .root = root, .bus = true});
}

}  // namespace

void SOUND::SESSION::mastered(Arrangement &tracks, Graph &wiring) {
  Whole master = NONE;
  for (Whole at = 0; at < wiring.nodes.size() && master == NONE; ++at)
    if (wiring.nodes[at].seat == ::BUSSED) master = at;
  if (master == NONE) {
    wiring.nodes.push_back(
      {.seat = ::BUSSED, .name = ::stamped(wiring, MASTER)});
    master = wiring.nodes.size() - 1;
  }
  ::untargeted(wiring, master);
  Node &root = wiring.nodes[master];
  root.seat = Node::ROOT;
  const String device = root.device;
  const Whole lane = root.lane;
  root.device.clear();
  root.lane = 0;
  const Node clock = {
    .seat = Node::PLUGIN,
    .name = ::stamped(wiring, ::CLOCKED),
    .plugin = ::CLOCKED,
    .claim = root.name,
    .device = device,
    .lane = lane,
    .clock = true};
  wiring.wires.push_back(
    {.from = root.name, .out = 0, .to = clock.name, .in = 0});
  ::tracked(tracks, root.name);
  wiring.nodes.push_back(clock);
}

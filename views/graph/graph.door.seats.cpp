// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../graph.hpp"
#include "../../history.hpp"
#include "../../kind.hpp"
#include "../../session.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot REFUSED = "that plugin would not be made";
constexpr STRING::Hot UNOPENED = "that device would not open";
constexpr STRING::Hot UNSENT = "the track has no audio out to send";
constexpr STRING::Hot UNHEARD = "that track admits no audio";

auto mouth(const String &root) -> Whole {
  const Node &held = GRAPH::held().nodes[GRAPH::at(root)];
  for (Whole in = 0; in < held.ins.size(); ++in)
    if (held.ins[in].kind == KIND::AUDIO) return in;
  return NONE;
}

auto covered(const String &node) -> Graph {
  Graph facts = {.nodes = {GRAPH::held().nodes[GRAPH::at(node)]}};
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == node || wire.to == node) facts.wires.push_back(wire);
  return facts;
}

}  // namespace

void SOUND::VIEWS::WIRED::take(const String &plugin) {
  const String node = GRAPH::seat(plugin);
  if (!node.empty()) {
    SESSION::seated(plugin);
    GRAPH::claim(node, steering());
    HISTORY::record({.act = "seat", .graph = ::covered(node)});
  }
  GUI::set(document(), NOTICE, GUI::Text{node.empty() ? ::REFUSED : ""});
}

void SOUND::VIEWS::WIRED::surface(
  const String &plugin, const String &device, Whole lane) {
  const String node = GRAPH::surface(plugin, device);
  if (!node.empty()) {
    GRAPH::claim(node, steering());
    if (lane != 0) GRAPH::lane(node, lane);
    HISTORY::record({.act = "surface", .graph = ::covered(node)});
  }
  GUI::set(document(), NOTICE, GUI::Text{node.empty() ? ::UNOPENED : ""});
}

void SOUND::VIEWS::WIRED::send(const String &root) {
  const String steered = steering();
  if (steered.empty()) return;
  const Vector<Port> &outs = GRAPH::held().nodes[GRAPH::at(steered)].outs;
  Whole out = NONE;
  for (Whole port = 0; port < outs.size() && out == NONE; ++port)
    if (outs[port].kind == KIND::AUDIO) out = port;
  const Whole in = ::mouth(root);
  const STRING::Hot refused = out == NONE ? ::UNSENT
                              : in == NONE
                                ? ::UNHEARD
                                : GRAPH::refusal(steered, out, root, in);
  if (refused[0] == '\0') {
    GRAPH::hookup(steered, out, root, in);
    HISTORY::record(
      {.act = "wire", .graph = {.wires = {GRAPH::held().wires.back()}}});
  }
  GUI::set(document(), NOTICE, GUI::Text{refused});
}

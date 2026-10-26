// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../history.hpp"
#include "graph.door.wired.hpp"
#include "graph.keys.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WIRE = "wire";
constexpr STRING::Hot UNWIRE = "unwire";

auto first(const Vector<Port> &ports, Whole kind) -> Whole {
  for (Whole port = 0; port < ports.size(); ++port)
    if (ports[port].kind == kind) return port;
  return NONE;
}

auto held(const String &node) -> const Node & {
  return GRAPH::held().nodes[GRAPH::at(node)];
}

auto fed(const String &from, Whole out, const String &plug) -> Vector<Wire> {
  Vector<Wire> wires;
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == from && wire.out == out && wire.to != plug)
      wires.push_back(wire);
  return wires;
}

auto end(const String &plug, Whole kind) -> String {
  const String root = VIEWS::WIRED::steering();
  if (root.empty()) return {};
  const auto on = [&root](const String &node) {
    return node == root || GRAPH::claimed(node) == root;
  };
  const auto inside = [&on](const Wire &wire) { return on(wire.to); };
  String last;
  for (const Node &node : GRAPH::held().nodes) {
    const Whole out = ::first(node.outs, kind);
    if (out == NONE || node.name == plug || !on(node.name)) continue;
    if (std::ranges::none_of(::fed(node.name, out, plug), inside))
      last = node.name;
  }
  return last;
}

auto hooked(const Wire &wire) -> STRING::Hot {
  if (!GRAPH::hookup(wire.from, wire.out, wire.to, wire.in))
    return GRAPH::refusal(wire.from, wire.out, wire.to, wire.in);
  HISTORY::record({.act = ::WIRE, .graph = {.wires = {wire}}});
  return "";
}

auto inserted(const Wire &taken, Whole out) -> STRING::Hot {
  for (const Wire &wire : ::fed(taken.from, taken.out, taken.to)) {
    const Wire drawn = {taken.to, out, wire.to, wire.in};
    const STRING::Hot why = GRAPH::refusal(drawn.from, out, wire.to, wire.in);
    if (why[0] != '\0') return why;
    GRAPH::unwire(wire.from, wire.out, wire.to, wire.in);
    HISTORY::record(
      {.act = ::UNWIRE, .verb = Edit::REMOVED, .graph = {.wires = {wire}}});
    ::hooked(drawn);
  }
  return "";
}

auto joined(const String &plug, Whole kind) -> STRING::Hot {
  const String end = ::end(plug, kind);
  if (end.empty()) return "";
  const Whole out = ::first(::held(plug).outs, kind);
  STRING::Hot why = "";
  for (const Wire &wire : ::fed(end, ::first(::held(end).outs, kind), plug))
    if (why[0] == '\0') why = ::hooked({plug, out, wire.to, wire.in});
  return why;
}

}  // namespace

auto SOUND::VIEWS::WIRED::wired(const String &node) -> STRING::Hot {
  const Vector<Whole> takes = GRAPH::takes(::held(node).plugin);
  if (takes.empty()) return "";
  const Whole kind = takes.front();
  const Vector<String> picked = chosen(document());
  const String feed = picked.size() == 1 ? picked.front() : ::end(node, kind);
  if (GRAPH::at(feed) == NONE) return "";
  const Wire taken = {
    feed, ::first(::held(feed).outs, kind), node,
    ::first(::held(node).ins, kind)};
  if (const STRING::Hot refused = ::hooked(taken); refused[0] != '\0')
    return refused;
  const Vector<Whole> gives = GRAPH::gives(::held(node).plugin);
  if (std::ranges::contains(gives, kind))
    return ::inserted(taken, ::first(::held(node).outs, kind));
  for (const Whole other : gives)
    if (other != kind) return ::joined(node, other);
  return "";
}

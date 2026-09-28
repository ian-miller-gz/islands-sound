// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto absent(const Vector<Port> &ports, Whole port) -> Flag {
  return port >= ports.size();
}

auto same(const Wire &held, const Wire &wire) -> Flag {
  return held.from == wire.from && held.out == wire.out && held.to == wire.to &&
         held.in == wire.in;
}

auto tracked(const Node &node) -> String {
  return node.seat == Node::ROOT ? node.name : node.claim;
}

auto strayed(const Node &source, const Node &sink) -> Flag {
  if (!GRAPH::surfaced(source) && !GRAPH::surfaced(sink)) return false;
  const String from = ::tracked(source), to = ::tracked(sink);
  return from.empty() || from != to;
}

auto crossed(const Node &source, const Node &sink) -> Flag {
  if (source.seat != Node::ROOT || sink.seat == Node::ROOT) return false;
  if (sink.seat == Node::RECORD) return false;
  return !sink.claim.empty() && sink.claim != source.name;
}

auto worded(STRING::Hot doing, Whole kind) -> STRING::Hot {
  static String said;
  said = std::format("lane {} is {}", KIND::spoken(kind), doing);
  return said.c_str();
}

auto drawn(const Wire &wire) -> Flag {
  for (const Wire &held : GRAPH::standing().wires)
    if (::same(held, wire)) return true;
  return false;
}

auto without(const String &gone) -> Vector<Wire> {
  Vector<Wire> kept;
  for (const Wire &wire : GRAPH::standing().wires)
    if (wire.from != gone && wire.to != gone) kept.push_back(wire);
  return kept;
}

void reclaimed(const String &gone) {
  for (Node &node : GRAPH::standing().nodes) {
    if (node.claim == gone) node.claim.clear();
    std::erase_if(
      node.berths, [&](const Berth &berth) { return berth.page == gone; });
  }
}

void plucked(const String &node) {
  Graph &wiring = GRAPH::standing();
  const Whole stood = GRAPH::at(node);
  Vector<GRAPH::Stand> &halves = GRAPH::stands();
  const GRAPH::Stand &stand = halves[stood];
  if (stand.instance != nullptr) stand.offer->surface->destroy(stand.instance);
  for (const Wire &wire : wiring.wires)
    if (wire.from == node && GRAPH::noted(node, wire.out)) GRAPH::hush(wire.to);
  wiring.wires = ::without(node);
  GRAPH::shed(node);
  ::reclaimed(node);
  wiring.nodes.erase(wiring.nodes.begin() + static_cast<Integer>(stood));
  halves.erase(halves.begin() + static_cast<Integer>(stood));
  GRAPH::stir();
}

void stripped(const String &root, Vector<String> &gone) {
  for (const Node &node : GRAPH::standing().nodes)
    if (node.claim == root) gone.push_back(node.name);
  for (const String &name : gone) ::plucked(name);
}

}  // namespace

auto SOUND::GRAPH::refusal(
  const String &from, Whole out, const String &to, Whole in) -> STRING::Hot {
  const Whole source = at(from), sink = at(to);
  if (source == NONE || sink == NONE) return "no such node";
  const Vector<Node> &nodes = standing().nodes;
  if (source == sink && out == in) return "a port cannot wire to itself";
  if (::absent(nodes[source].outs, out)) return "no such out";
  if (::absent(nodes[sink].ins, in)) return "no such in";
  if (nodes[source].outs[out].kind != nodes[sink].ins[in].kind)
    return "the ports carry different kinds";
  if (nodes[source].outs[out].type != nodes[sink].ins[in].type)
    return "the ports carry different types";
  if (::drawn({from, out, to, in})) return "already wired";
  if (::strayed(nodes[source], nodes[sink]))
    return "a surface stays in its own track";
  if (::crossed(nodes[source], nodes[sink]))
    return "a track's out is its own page's";
  if (nodes[sink].seat == Node::RECORD && blocked(nodes[sink].claim, in))
    return ::worded("sending", nodes[sink].ins[in].kind);
  if (
    nodes[source].seat == Node::ROOT && to != recorder(from) &&
    !bypassed(from, out) && joined(recorder(from), Side::IN, out) != 0)
    return ::worded("recording", nodes[source].outs[out].kind);
  if (circled({from, out, to, in})) return "the tracks would feed in a circle";
  return "";
}

auto SOUND::GRAPH::hookup(
  const String &from, Whole out, const String &to, Whole in) -> Flag {
  if (refusal(from, out, to, in)[0] != '\0') return false;
  standing().wires.push_back({from, out, to, in});
  shed(standing().wires.back());
  stir();
  return true;
}

auto SOUND::GRAPH::fed(const String &root, Whole out) -> String {
  const Wire *only = nullptr;
  for (const Wire &wire : standing().wires) {
    if (wire.from != root || wire.out != out) continue;
    if (only != nullptr) return {};
    only = &wire;
  }
  if (only == nullptr) return {};
  const Whole stood = at(only->to);
  return stood == NONE ? String() : standing().nodes[stood].plugin;
}

auto SOUND::GRAPH::noted(const String &node, Whole out) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE) return false;
  const Vector<Port> &outs = standing().nodes[stood].outs;
  return out < outs.size() && outs[out].kind == KIND::NOTES;
}

auto SOUND::GRAPH::unwire(
  const String &from, Whole out, const String &to, Whole in) -> Flag {
  Vector<Wire> &wires = standing().wires;
  for (Whole at = 0; at < wires.size(); ++at)
    if (::same(wires[at], {from, out, to, in})) {
      if (noted(from, out)) hush(to);
      wires.erase(wires.begin() + static_cast<Integer>(at));
      stir();
      return true;
    }
  return false;
}

auto SOUND::GRAPH::unseat(const String &node) -> Flag {
  Vector<String> gone;
  return unseat(node, gone);
}

auto SOUND::GRAPH::unseat(const String &node, Vector<String> &gone) -> Flag {
  gone.clear();
  const Whole seated = at(node);
  if (seated == NONE) return false;
  if (standing().nodes[seated].seat == Node::RECORD) return false;
  if (standing().nodes[seated].seat == Node::ROOT) ::stripped(node, gone);
  gone.push_back(node);
  ::plucked(node);
  return true;
}

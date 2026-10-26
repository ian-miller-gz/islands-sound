// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"
#include "../kind.hpp"

namespace {
using namespace SOUND;

Graph wiring;
Vector<GRAPH::Stand> halves;
Whole counted = 0;

auto ported(const Vector<AUDIO::PLUGIN::Port> &declared) -> Vector<Port> {
  Vector<Port> ports;
  for (const AUDIO::PLUGIN::Port &port : declared)
    ports.push_back(
      {.kind = port.kind,
       .name = port.name.empty() ? String(KIND::spoken(port.kind)) : port.name,
       .type = port.type});
  return ports;
}

}  // namespace

auto SOUND::GRAPH::added(const Node &node, const Stand &stand) -> String {
  ::wiring.nodes.push_back(node);
  ::halves.push_back(stand);
  stir();
  return node.name;
}

auto SOUND::GRAPH::standing() -> Graph & { return ::wiring; }
auto SOUND::GRAPH::stands() -> Vector<Stand> & { return ::halves; }
auto SOUND::GRAPH::held() -> const Graph & { return ::wiring; }
auto SOUND::GRAPH::counted() -> Whole { return ::counted; }
void SOUND::GRAPH::stir() {
  ++::counted;
  reckon();
  owe();
}

auto SOUND::GRAPH::seat(const String &plugin) -> String {
  return seat(plugin, plugin);
}

auto SOUND::GRAPH::seat(const String &plugin, const String &name) -> String {
  const PLUGIN::Offer *row = PLUGIN::found(plugin);
  if (row == nullptr) return {};
  void *instance = row->surface->create(RATE, CHANNELS);
  if (instance == nullptr) return {};
  return added(
    {.seat = Node::PLUGIN,
     .name = stamped(name),
     .plugin = plugin,
     .ins = ::ported(row->surface->ins),
     .outs = ::ported(row->surface->outs)},
    {.offer = row, .instance = instance});
}

auto SOUND::GRAPH::root(const Vector<Whole> &kinds) -> String {
  return root(kinds, "root");
}

auto SOUND::GRAPH::root(const Vector<Whole> &kinds, const String &name)
  -> String {
  const String node = added({.seat = Node::ROOT, .name = stamped(name)}, {});
  for (Whole kind : kinds) grow(node, kind);
  return node;
}

auto SOUND::GRAPH::grow(const String &root, Whole kind) -> Whole {
  if (!rooted(root)) return NONE;
  const Whole node = at(root);
  Vector<Port> &outs = ::wiring.nodes[node].outs;
  outs.push_back({.kind = kind, .name = KIND::spoken(kind)});
  for (Tape &tape : ::halves[node].tapes) tape.runs.push_back({});
  const String taking = recorder(root);
  if (!taking.empty())
    ::wiring.nodes[at(taking)].ins.push_back(mirrored(outs.back()));
  return outs.size() - 1;
}

auto SOUND::GRAPH::rooted(const String &node) -> Flag {
  const Whole stood = at(node);
  return stood != NONE && ::wiring.nodes[stood].seat == Node::ROOT;
}

auto SOUND::GRAPH::offered(const String &node) -> const PLUGIN::Offer * {
  const Whole stood = at(node);
  return stood != NONE ? ::halves[stood].offer : nullptr;
}

auto SOUND::GRAPH::instance(const String &node) -> void * {
  const Whole stood = at(node);
  return stood != NONE ? ::halves[stood].instance : nullptr;
}

void SOUND::GRAPH::rest() {
  for (Stand &stand : ::halves) {
    if (stand.offer == nullptr) continue;
    if (stand.instance != nullptr)
      stand.offer->surface->destroy(stand.instance);
    stand.instance = stand.offer->surface->create(RATE, CHANNELS);
  }
  rebind();
}

void SOUND::GRAPH::close() {
  unprobed();
  for (const Stand &stand : ::halves)
    if (stand.instance != nullptr)
      stand.offer->surface->destroy(stand.instance);
  ::halves.clear();
  ::wiring = {};
  stir();
}

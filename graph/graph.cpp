// SPDX-License-Identifier: AGPL-3.0-or-later
#include <threads.hpp>

#include "graph.internal.hpp"
#include "../kind.hpp"

namespace {
using namespace SOUND;

Graph wiring;
Vector<GRAPH::Stand *> seated;
Whole counted = 0;
THREADS::Shared<Whole> mirrored{0};
thread_local Flag touched = false;

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
  ::seated.push_back(&GRAPH::slot(stand));
  stir();
  return node.name;
}

auto SOUND::GRAPH::standing() -> Graph & {
  ::touched = true;
  return ::wiring;
}
auto SOUND::GRAPH::stand(Whole at) -> Stand & {
  ::touched = true;
  return *::seated[at];
}
auto SOUND::GRAPH::GET::seated() -> const Vector<Stand *> & { return ::seated; }
void SOUND::GRAPH::retire(Whole at) {
  retire(*::seated[at]);
  ::seated.erase(::seated.begin() + static_cast<Integer>(at));
}
auto SOUND::GRAPH::held() -> const Graph & {
  ::touched = true;
  return ::wiring;
}
auto SOUND::GRAPH::counted() -> Whole { return ::counted; }
auto SOUND::GRAPH::SIGNAL::counted() -> Whole { return ::mirrored.load(); }
void SOUND::GRAPH::SIGNAL::clean() { ::touched = false; }
auto SOUND::GRAPH::SIGNAL::touched() -> Flag { return ::touched; }
void SOUND::GRAPH::stir() {
  ++::counted;
  ::mirrored.store(::counted);
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

void SOUND::GRAPH::close() {
  unprobed();
  for (Stand *stand : ::seated) retire(*stand);
  reap();
  ::seated.clear();
  ::wiring = {};
  stir();
}

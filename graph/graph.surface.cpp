// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto sinking() -> const PLUGIN::Offer * {
  for (const PLUGIN::Offer &row : PLUGIN::catalog())
    if (
      row.bind != nullptr && !row.surface->ins.empty() &&
      row.surface->ins[0].kind == KIND::AUDIO)
      return &row;
  return nullptr;
}

}  // namespace

auto SOUND::GRAPH::surfaced(const Node &node) -> Flag {
  return node.seat == Node::PLUGIN && (!node.device.empty() || node.clock);
}

auto SOUND::GRAPH::surface(const String &plugin, const String &device) -> String {
  return surface(plugin, device, plugin);
}

auto SOUND::GRAPH::surface(
  const String &plugin, const String &device, const String &name) -> String {
  const PLUGIN::Offer *row = PLUGIN::found(plugin);
  if (row == nullptr || row->bind == nullptr || device.empty()) return {};
  const String node = seat(plugin, name);
  if (node.empty()) return {};
  if (!row->bind(instance(node), device, 0)) {
    unseat(node);
    return {};
  }
  standing().nodes[at(node)].device = device;
  return node;
}

auto SOUND::GRAPH::clock() -> String {
  for (const Node &node : held().nodes)
    if (node.clock) return node.name;
  return {};
}

auto SOUND::GRAPH::clock(const String &device, const String &name) -> String {
  const PLUGIN::Offer *row = ::sinking();
  if (row == nullptr) return {};
  const String node = seat(row->name, name);
  if (node.empty()) return {};
  for (Node &held : standing().nodes) held.clock = false;
  Node &stood = standing().nodes[at(node)];
  stood.device = device;
  stood.clock = true;
  return node;
}

void SOUND::GRAPH::rebind() {
  const Vector<Node> &nodes = held().nodes;
  Vector<Stand> &halves = stands();
  for (Whole row = 0; row < nodes.size() && row < halves.size(); ++row) {
    if (nodes[row].clock || nodes[row].device.empty()) continue;
    if (halves[row].instance == nullptr) continue;
    const PLUGIN::Offer *offer = halves[row].offer;
    if (offer != nullptr && offer->bind != nullptr)
      offer->bind(halves[row].instance, nodes[row].device, nodes[row].lane);
  }
}

auto SOUND::GRAPH::lane(const String &node, Whole lane) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE) return false;
  Node &held = standing().nodes[stood];
  if (held.device.empty() && !held.clock) return false;
  held.lane = lane;
  if (held.clock) return true;
  Stand &stand = stands()[stood];
  if (stand.offer == nullptr || stand.offer->bind == nullptr) return false;
  if (stand.instance != nullptr) stand.offer->surface->destroy(stand.instance);
  stand.instance = stand.offer->surface->create(RATE, CHANNELS);
  return stand.instance != nullptr &&
         stand.offer->bind(stand.instance, held.device, lane);
}

void SOUND::GRAPH::main(const String &device) {
  const Whole stood = at(clock());
  if (stood != NONE) standing().nodes[stood].device = device;
}

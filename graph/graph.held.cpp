// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.held.hpp"
#include "graph.internal.hpp"

auto SOUND::GRAPH::hold(
  const String &plugin, const String &name, const Vector<Port> &ins,
  const Vector<Port> &outs) -> String {
  return added(
    {.seat = Node::PLUGIN,
     .name = stamped(name),
     .plugin = plugin,
     .ins = ins,
     .outs = outs},
    {});
}

auto SOUND::GRAPH::missing(const String &node) -> Flag {
  const Whole stood = at(node);
  return stood != NONE && held().nodes[stood].seat == Node::PLUGIN &&
         stand(stood).offer == nullptr;
}

auto SOUND::GRAPH::rooted(const String &node) -> Flag {
  const Whole stood = at(node);
  return stood != NONE && held().nodes[stood].seat == Node::ROOT;
}

auto SOUND::GRAPH::offered(const String &node) -> const PLUGIN::Offer * {
  const Whole stood = at(node);
  return stood != NONE ? stand(stood).offer : nullptr;
}

auto SOUND::GRAPH::instance(const String &node) -> void * {
  const Whole stood = at(node);
  return stood != NONE ? stand(stood).instance : nullptr;
}

auto SOUND::GRAPH::PLAN::offered(Whole node) -> const PLUGIN::Offer * {
  const Vector<Stand *> &stands = played().stands;
  return node < stands.size() ? stands[node]->offer : nullptr;
}

auto SOUND::GRAPH::PLAN::instance(Whole node) -> void * {
  const Vector<Stand *> &stands = played().stands;
  return node < stands.size() ? stands[node]->instance : nullptr;
}

void SOUND::GRAPH::listen() {
  for (Whole node = 0; node < held().nodes.size(); ++node) {
    const Stand &stand = GRAPH::stand(node);
    if (stand.offer == nullptr || stand.instance == nullptr) continue;
    if (stand.offer->listen != nullptr) stand.offer->listen(stand.instance);
  }
}

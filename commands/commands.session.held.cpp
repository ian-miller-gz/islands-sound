// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "../kind.hpp"
#include "commands.held.hpp"

namespace {
using namespace SOUND;

struct End {
  String node;
  Side side = Side::OUT;
  Whole port = 0;
};

auto declared(const Node &planned, const End &end) -> Port {
  const PLUGIN::Offer *row = PLUGIN::found(planned.plugin);
  if (row == nullptr) return {};
  const Vector<AUDIO::PLUGIN::Port> &ports =
    end.side == Side::IN ? row->surface->ins : row->surface->outs;
  if (end.port >= ports.size()) return {};
  return {.kind = ports[end.port].kind, .type = ports[end.port].type};
}

auto far(const End &end, const Graph &plan) -> Port {
  const Whole stood = GRAPH::at(end.node);
  if (stood != NONE) {
    const Node &node = GRAPH::held().nodes[stood];
    const Vector<Port> &ports = end.side == Side::IN ? node.ins : node.outs;
    if (end.port < ports.size()) return ports[end.port];
  }
  for (const Node &planned : plan.nodes)
    if (planned.name == end.node) return ::declared(planned, end);
  return {};
}

void worn(Vector<Port> &ports, Whole port, const Port &end) {
  if (port >= ports.size()) ports.resize(port + 1);
  ports[port] = {.kind = end.kind, .type = end.type};
}

auto spoken(Vector<Port> ports) -> Vector<Port> {
  for (Port &port : ports) port.name = KIND::spoken(port.kind);
  return ports;
}

}  // namespace

auto SOUND::COMMANDS::held(const Node &planned, const Graph &plan) -> String {
  const String seated = GRAPH::seat(planned.plugin, planned.name);
  if (!seated.empty() || planned.plugin.empty()) return seated;
  if (PLUGIN::found(planned.plugin) != nullptr) return seated;
  Vector<Port> ins, outs;
  for (const Wire &wire : plan.wires) {
    if (wire.to == planned.name)
      ::worn(ins, wire.in, ::far({wire.from, Side::OUT, wire.out}, plan));
    if (wire.from == planned.name)
      ::worn(outs, wire.out, ::far({wire.to, Side::IN, wire.in}, plan));
  }
  return GRAPH::hold(
    planned.plugin, planned.name, ::spoken(ins), ::spoken(outs));
}

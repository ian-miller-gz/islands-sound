// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto sent(const String &root, Whole lane) -> Whole {
  Whole count = 0;
  const String own = GRAPH::recorder(root);
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == root && wire.out == lane && wire.to != own) ++count;
  return count;
}

auto stripped(const Vector<Port> &outs) -> Vector<Port> {
  Vector<Port> ins;
  for (const Port &out : outs) ins.push_back(GRAPH::mirrored(out));
  return ins;
}

}  // namespace

auto SOUND::GRAPH::record(const String &root) -> String {
  return record(root, RECORDED);
}

auto SOUND::GRAPH::record(const String &root, const String &name) -> String {
  if (!rooted(root) || !recorder(root).empty()) return {};
  return added(
    {.seat = Node::RECORD,
     .name = stamped(name),
     .ins = ::stripped(standing().nodes[at(root)].outs),
     .claim = root},
    {});
}

auto SOUND::GRAPH::recorder(const String &root) -> String {
  if (root.empty()) return {};
  for (const Node &node : held().nodes)
    if (node.seat == Node::RECORD && node.claim == root) return node.name;
  return {};
}

auto SOUND::GRAPH::blocked(const String &root, Whole lane) -> Flag {
  return rooted(root) && ::sent(root, lane) != 0 && !bypassed(root, lane);
}

auto SOUND::GRAPH::mirrored(const Port &port) -> Port {
  return {.kind = port.kind, .name = port.name, .type = port.type};
}

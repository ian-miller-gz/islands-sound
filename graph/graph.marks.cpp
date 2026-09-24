// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

auto SOUND::GRAPH::claim(const String &node, const String &root) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE || !rooted(root)) return false;
  Vector<Node> &nodes = standing().nodes;
  if (nodes[stood].seat == Node::ROOT) return false;
  nodes[stood].claim = root;
  return true;
}

auto SOUND::GRAPH::claimed(const String &node) -> String {
  const Whole stood = at(node);
  return stood != NONE ? standing().nodes[stood].claim : String();
}

auto SOUND::GRAPH::quiet(const String &node, Flag on) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE) return false;
  Vector<Node> &nodes = standing().nodes;
  if (nodes[stood].seat == Node::ROOT) return false;
  if (on && !nodes[stood].quiet)
    for (const Wire &wire : standing().wires)
      if (wire.from == node && noted(node, wire.out)) hush(wire.to);
  nodes[stood].quiet = on;
  return true;
}

auto SOUND::GRAPH::quieted(const String &node) -> Flag {
  const Whole stood = at(node);
  return stood != NONE && standing().nodes[stood].quiet;
}

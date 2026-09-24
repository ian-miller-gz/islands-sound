// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../kind.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto onto(const Wire &wire, const String &node, Whole in) -> Flag {
  return wire.to == node && wire.in == in;
}

auto off(const Wire &wire, const String &node, Whole out) -> Flag {
  return wire.from == node && wire.out == out;
}

void parted(const String &root, Whole out, Whole in) {
  Graph &wiring = GRAPH::standing();
  const String taking = GRAPH::recorder(root);
  const auto cut = [&](const Wire &wire) {
    return ::off(wire, root, out) || ::onto(wire, taking, out) ||
           (in != NONE && ::onto(wire, root, in));
  };
  if (GRAPH::noted(root, out))
    for (const Wire &wire : wiring.wires)
      if (::off(wire, root, out)) GRAPH::hush(wire.to);
  std::erase_if(wiring.wires, cut);
  std::erase_if(wiring.slack, cut);
}

}  // namespace

auto SOUND::GRAPH::recast(const String &root, Whole out, Whole kind) -> Flag {
  if (!rooted(root) || kind > KIND::DATA) return false;
  Node &held = standing().nodes[at(root)];
  if (out >= held.outs.size()) return false;
  if (held.outs[out].kind == kind) return true;
  const Whole in = admitted(root, out);
  ::parted(root, out, in);
  held.outs[out].kind = kind;
  held.outs[out].name = KIND::spoken(kind);
  held.outs[out].type.clear();
  if (in != NONE) {
    held.ins[in] = mirrored(held.outs[out]);
    held.ins[in].lane = out;
  }
  const String taking = recorder(root);
  if (!taking.empty())
    standing().nodes[at(taking)].ins[out] = mirrored(held.outs[out]);
  stir();
  return true;
}

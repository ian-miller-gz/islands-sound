// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto shorn(const String &root, Whole out) -> Vector<Wire> {
  Vector<Wire> kept;
  for (const Wire &wire : GRAPH::standing().wires) {
    if (wire.from == root && wire.out == out) continue;
    Wire held = wire;
    if (held.from == root && held.out > out) --held.out;
    kept.push_back(held);
  }
  return kept;
}

auto reaped(const String &taking, Whole in) -> Vector<Wire> {
  Vector<Wire> kept;
  for (const Wire &wire : GRAPH::standing().wires) {
    if (wire.to == taking && wire.in == in) continue;
    Wire held = wire;
    if (held.to == taking && held.in > in) --held.in;
    kept.push_back(held);
  }
  return kept;
}

void unfed(const String &root, Whole out) {
  const Whole in = GRAPH::admitted(root, out);
  Vector<Port> &ins = GRAPH::standing().nodes[GRAPH::at(root)].ins;
  if (in != NONE) {
    GRAPH::standing().wires = ::reaped(root, in);
    ins.erase(ins.begin() + static_cast<Integer>(in));
  }
  for (Port &port : ins)
    if (port.lane != NONE && port.lane > out) --port.lane;
}

void unmirrored(const String &root, Whole out) {
  const String taking = GRAPH::recorder(root);
  if (taking.empty()) return;
  GRAPH::standing().wires = ::reaped(taking, out);
  Vector<Port> &ins = GRAPH::standing().nodes[GRAPH::at(taking)].ins;
  if (out < ins.size()) ins.erase(ins.begin() + static_cast<Integer>(out));
}

}  // namespace

auto SOUND::GRAPH::shed(const String &root, Whole out) -> Flag {
  if (!rooted(root)) return false;
  const Whole stood = at(root);
  Vector<Port> &outs = standing().nodes[stood].outs;
  if (out >= outs.size()) return false;
  if (noted(root, out))
    for (const Wire &wire : standing().wires)
      if (wire.from == root && wire.out == out) hush(wire.to);
  standing().wires = ::shorn(root, out);
  ::unfed(root, out);
  ::unmirrored(root, out);
  outs.erase(outs.begin() + static_cast<Integer>(out));
  for (Tape &tape : stands()[stood].tapes)
    tape.runs.erase(tape.runs.begin() + static_cast<Integer>(out));
  stir();
  return true;
}

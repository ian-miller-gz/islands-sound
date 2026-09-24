// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

void shifted(const String &root, Whole out) {
  for (Wire &wire : GRAPH::standing().wires)
    if (wire.from == root && wire.out >= out) ++wire.out;
  for (Port &port : GRAPH::standing().nodes[GRAPH::at(root)].ins)
    if (port.lane != NONE && port.lane >= out) ++port.lane;
}

void mirrored(const String &root, const Port &port, Whole out) {
  const String taking = GRAPH::recorder(root);
  if (taking.empty()) return;
  for (Wire &wire : GRAPH::standing().wires)
    if (wire.to == taking && wire.in >= out) ++wire.in;
  Vector<Port> &ins = GRAPH::standing().nodes[GRAPH::at(taking)].ins;
  if (out > ins.size()) return;
  ins.insert(ins.begin() + static_cast<Integer>(out), GRAPH::mirrored(port));
}

}  // namespace

auto SOUND::GRAPH::grow(const String &root, const Port &port, Whole out)
  -> Whole {
  if (!rooted(root)) return NONE;
  const Whole stood = at(root);
  Vector<Port> &outs = standing().nodes[stood].outs;
  if (out > outs.size()) return NONE;
  ::shifted(root, out);
  ::mirrored(root, port, out);
  outs.insert(outs.begin() + static_cast<Integer>(out), port);
  for (Tape &tape : stands()[stood].tapes)
    tape.runs.insert(tape.runs.begin() + static_cast<Integer>(out), {});
  stir();
  return out;
}

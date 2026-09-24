// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

struct Ends {
  Whole from = NONE, to = NONE;
};

Vector<Flag> flags;
Vector<Ends> joins;
Vector<Flag> seen;
Vector<Whole> ahead;

auto reaches(Whole from, Whole to, Whole wire) -> Flag {
  ::seen.assign(GRAPH::held().nodes.size(), false);
  ::ahead.assign(1, to);
  while (!::ahead.empty()) {
    const Whole node = ::ahead.back();
    ::ahead.pop_back();
    if (node == from) return true;
    if (::seen[node]) continue;
    ::seen[node] = true;
    for (Whole row = 0; row < wire; ++row)
      if (!::flags[row] && ::joins[row].from == node && ::joins[row].to != NONE)
        ::ahead.push_back(::joins[row].to);
  }
  return false;
}

}  // namespace

void SOUND::GRAPH::reckon() {
  const Vector<Wire> &wires = held().wires;
  ::joins.clear();
  for (const Wire &wire : wires)
    ::joins.push_back({at(wire.from), at(wire.to)});
  ::flags.assign(wires.size(), false);
  for (Whole wire = 0; wire < wires.size(); ++wire) {
    const Ends &ends = ::joins[wire];
    if (ends.from == NONE || ends.to == NONE) continue;
    ::flags[wire] = ::reaches(ends.from, ends.to, wire);
  }
}

auto SOUND::GRAPH::dormant() -> const Vector<Flag> & { return ::flags; }

auto SOUND::GRAPH::dormant(const Wire &wire) -> Flag {
  const Vector<Wire> &wires = held().wires;
  for (Whole row = 0; row < wires.size() && row < ::flags.size(); ++row)
    if (
      wires[row].from == wire.from && wires[row].out == wire.out &&
      wires[row].to == wire.to && wires[row].in == wire.in)
      return ::flags[row];
  return false;
}

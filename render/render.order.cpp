// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"

namespace {
using namespace SOUND;

struct Ends {
  Whole from = NONE, to = NONE;
};

Vector<Ends> joins;
Vector<Flag> taken;
Vector<Whole> owing;
Vector<Whole> walk;
Whole counted = NONE;
Flag whole = false;

void ends() {
  joins.clear();
  const Vector<Wire> &wires = GRAPH::held().wires;
  const Vector<Flag> &dormant = GRAPH::dormant();
  for (Whole row = 0; row < wires.size() && row < dormant.size(); ++row)
    if (!dormant[row])
      joins.push_back({GRAPH::at(wires[row].from), GRAPH::at(wires[row].to)});
}

void owed() {
  owing.assign(taken.size(), 0);
  for (const Ends &wire : joins)
    if (wire.from != NONE && wire.to != NONE && !taken[wire.from])
      ++owing[wire.to];
}

auto ready() -> Whole {
  for (Whole node = 0; node < taken.size(); ++node)
    if (!taken[node] && owing[node] == 0) return node;
  return NONE;
}

auto built() -> Flag {
  const Whole nodes = GRAPH::held().nodes.size();
  ::ends();
  taken.assign(nodes, false);
  walk.clear();
  while (walk.size() < nodes) {
    ::owed();
    const Whole next = ::ready();
    if (next == NONE) {
      walk.clear();
      return false;
    }
    taken[next] = true;
    walk.push_back(next);
  }
  return true;
}

void fresh() {
  if (::counted == GRAPH::counted()) return;
  ::whole = ::built();
  ::counted = GRAPH::counted();
}

}  // namespace

auto SOUND::RENDER::order() -> const Vector<Whole> & {
  ::fresh();
  return ::walk;
}

auto SOUND::RENDER::ordered() -> Flag {
  ::fresh();
  return ::whole;
}

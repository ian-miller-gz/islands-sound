// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

void SOUND::GRAPH::owe(const String &node) {
  const Whole stood = at(node);
  if (stood != NONE) ++stands()[stood].stirred;
}

void SOUND::GRAPH::owe() {
  for (Stand &stand : stands()) ++stand.stirred;
}

auto SOUND::GRAPH::owed() -> Vector<String> {
  Vector<String> nodes;
  const Vector<Node> &standing = held().nodes;
  for (Whole node = 0; node < standing.size() && node < stands().size();
       ++node) {
    Stand &stand = stands()[node];
    if (written(stand).stamp != stand.stirred)
      nodes.push_back(standing[node].name);
  }
  return nodes;
}

void SOUND::GRAPH::threaded(const String &node) {
  const Whole stood = at(node);
  if (stood == NONE) return;
  Stand &stand = stands()[stood];
  written(stand).stamp = stand.stirred;
}

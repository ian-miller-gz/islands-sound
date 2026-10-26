// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

void SOUND::GRAPH::owe(const String &node) {
  const Whole stood = at(node);
  if (stood != NONE) ++stand(stood).stirred;
}

void SOUND::GRAPH::owe() {
  for (Whole node = 0; node < held().nodes.size(); ++node)
    ++stand(node).stirred;
}

auto SOUND::GRAPH::owed() -> Vector<String> {
  Vector<String> nodes;
  const Vector<Node> &standing = held().nodes;
  for (Whole node = 0; node < standing.size(); ++node) {
    Stand &half = stand(node);
    if (written(half).stamp != half.stirred)
      nodes.push_back(standing[node].name);
  }
  return nodes;
}

void SOUND::GRAPH::threaded(const String &node) {
  const Whole stood = at(node);
  if (stood == NONE) return;
  Stand &stand = GRAPH::stand(stood);
  written(stand).stamp = stand.stirred;
}

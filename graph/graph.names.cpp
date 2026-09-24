// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto worn(const String &name) -> Flag { return GRAPH::at(name) != NONE; }

}  // namespace

auto SOUND::GRAPH::at(const String &name) -> Whole {
  if (name.empty()) return NONE;
  const Vector<Node> &nodes = held().nodes;
  for (Whole node = 0; node < nodes.size(); ++node)
    if (nodes[node].name == name) return node;
  return NONE;
}

auto SOUND::GRAPH::stamped(const String &word) -> String {
  if (!::worn(word)) return word;
  for (Whole count = 2;; ++count) {
    const String said = std::format("{}{}", word, count);
    if (!::worn(said)) return said;
  }
}

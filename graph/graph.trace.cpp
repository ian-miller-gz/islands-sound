// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto fed(const String &node) -> Vector<String> {
  Vector<String> next;
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == node) next.push_back(wire.to);
  return next;
}

auto feeding(const String &node) -> Vector<String> {
  Vector<String> next;
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.to == node) next.push_back(wire.from);
  return next;
}

auto mouth(const Node &node) -> Flag { return node.seat != Node::PLUGIN; }

auto carries(const String &node, const String &root) -> Flag {
  return node == root || !::mouth(GRAPH::held().nodes[GRAPH::at(node)]);
}

auto joins(
  const Node &next, const String &member, const String &root,
  Flag downstream) -> Flag {
  if (::mouth(next)) return downstream;
  if (!downstream && member == root) return next.claim == root;
  return next.claim.empty() || next.claim == root;
}

auto taken(const Vector<GRAPH::TRACE::Step> &walked, const String &node)
  -> Flag {
  for (const GRAPH::TRACE::Step &step : walked)
    if (step.node == node) return true;
  return false;
}

}  // namespace

auto SOUND::GRAPH::TRACE::trace(const String &root) -> Vector<Step> {
  Vector<Step> walked;
  if (!rooted(root)) return walked;
  walked.push_back({root, 0, 0});
  Whole branches = 0;
  for (Whole seen = 0; seen < walked.size(); ++seen) {
    const Step step = walked[seen];
    if (!::carries(step.node, root)) continue;
    Flag first = true;
    const auto join = [&](const String &next, Flag downstream) {
      if (::taken(walked, next)) return;
      if (!::joins(held().nodes[at(next)], step.node, root, downstream)) return;
      walked.push_back(
        {next, first ? step.branch : ++branches, step.depth + 1});
      first = false;
    };
    for (const String &next : ::fed(step.node)) join(next, true);
    for (const String &next : ::feeding(step.node)) join(next, false);
  }
  return walked;
}

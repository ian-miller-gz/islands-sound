// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "graph.internal.hpp"

auto SOUND::VIEWS::WIRED::picture() -> Vector<Seen> {
  const Vector<Node> &nodes = GRAPH::held().nodes;
  Vector<Seen> seen(nodes.size());
  const String root = steering();
  if (root.empty()) {
    for (Seen &one : seen) one.stands = true;
    return seen;
  }
  Vector<Flag> traced(seen.size(), false);
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const GRAPH::TRACE::Step &step : GRAPH::TRACE::trace(track.root))
      traced[GRAPH::at(step.node)] = true;
  for (Whole node = 0; node < seen.size(); ++node) {
    const String claim = GRAPH::claimed(nodes[node].name);
    const Berth berth = GRAPH::berth(nodes[node].name, root);
    const Flag berthed = nodes[node].seat != Node::PLUG &&
                         nodes[node].name != root &&
                         berth.across != Float(NONE);
    seen[node].stands =
      claim == root || (claim.empty() && !traced[node]) || berthed;
  }
  for (const GRAPH::TRACE::Step &step : GRAPH::TRACE::trace(root))
    seen[GRAPH::at(step.node)] = {true, step.branch, step.depth};
  return seen;
}

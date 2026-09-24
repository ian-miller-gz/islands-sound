// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto fed(const String &node, const Wire &candidate) -> Vector<String> {
  Vector<String> next;
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == node) next.push_back(wire.to);
  if (candidate.from == node) next.push_back(candidate.to);
  return next;
}

auto met(const Vector<String> &names, const String &name) -> Flag {
  for (const String &held : names)
    if (held == name) return true;
  return false;
}

auto aimed(const String &root, const Wire &candidate) -> Vector<String> {
  Vector<String> walked = {root}, aims;
  for (Whole seen = 0; seen < walked.size(); ++seen)
    for (const String &name : ::fed(walked[seen], candidate)) {
      if (::met(walked, name)) continue;
      const Whole stood = GRAPH::at(name);
      if (stood == NONE) continue;
      if (GRAPH::held().nodes[stood].seat != Node::ROOT) {
        walked.push_back(name);
        continue;
      }
      if (!::met(aims, name)) aims.push_back(name);
    }
  return aims;
}

auto kept(
  const Vector<String> &roots, const Vector<Vector<String>> &aims,
  const Vector<Flag> &struck, Whole at) -> Flag {
  for (Whole by = 0; by < roots.size(); ++by)
    if (!struck[by] && ::met(aims[by], roots[at])) return true;
  return false;
}

}  // namespace

auto SOUND::GRAPH::circled(const Wire &candidate) -> Flag {
  Vector<String> roots;
  Vector<Vector<String>> aims;
  for (const Node &node : held().nodes)
    if (node.seat == Node::ROOT) {
      roots.push_back(node.name);
      aims.push_back(::aimed(node.name, candidate));
    }
  Vector<Flag> struck(roots.size(), false);
  for (Flag moved = true; moved;) {
    moved = false;
    for (Whole at = 0; at < roots.size(); ++at) {
      if (struck[at] || ::kept(roots, aims, struck, at)) continue;
      struck[at] = true;
      moved = true;
    }
  }
  for (Flag done : struck)
    if (!done) return true;
  return false;
}

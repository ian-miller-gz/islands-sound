// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../control.hpp"
#include "../../graph.hpp"
#include "../../history.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto paged(const String &node) -> String {
  return GRAPH::rooted(node) ? node : GRAPH::claimed(node);
}

auto between(const Wire &wire, const String &root, const String &page) -> Flag {
  return (wire.from == root && ::paged(wire.to) == page) ||
         (wire.to == root && ::paged(wire.from) == page);
}

}  // namespace

auto SOUND::VIEWS::WIRED::unseat(const String &node) -> Flag {
  if (GRAPH::at(node) == NONE || GRAPH::rooted(node)) return false;
  Graph facts = {.nodes = {GRAPH::held().nodes[GRAPH::at(node)]}};
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == node || wire.to == node) facts.wires.push_back(wire);
  Edit edit = {.act = "unseat", .verb = Edit::REMOVED, .graph = facts};
  if (!GRAPH::unseat(node, edit.swept)) return false;
  CONTROL::settle(edit.swept);
  HISTORY::record(edit);
  return true;
}

auto SOUND::VIEWS::WIRED::dismiss(const String &root) -> Flag {
  const String page = steering();
  if (page.empty() || root == page || !GRAPH::rooted(root)) return false;
  Vector<Wire> cut;
  for (const Wire &wire : GRAPH::held().wires)
    if (::between(wire, root, page)) cut.push_back(wire);
  for (const Wire &wire : cut)
    GRAPH::unwire(wire.from, wire.out, wire.to, wire.in);
  GRAPH::unberth(root, page);
  if (!cut.empty())
    HISTORY::record(
      {.act = "unwire", .verb = Edit::REMOVED, .graph = {.wires = cut}});
  return true;
}

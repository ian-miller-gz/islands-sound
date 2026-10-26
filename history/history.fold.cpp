// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "history.internal.hpp"

namespace {
using namespace SOUND;

template <typename Fact>
void joined(Vector<Fact> &into, const Vector<Fact> &from) {
  into.insert(into.end(), from.begin(), from.end());
}

void joined(Vector<Change> &into, const Vector<Change> &from) {
  for (const Change &one : from) {
    Flag stood = false;
    for (Change &held : into)
      if (held.word == one.word && held.at == one.at) {
        held.now = one.now;
        stood = true;
      }
    if (!stood) into.push_back(one);
  }
}

template <typename Fact>
void refreshed(Vector<Fact> &into, const Vector<Fact> &from) {
  for (const Fact &one : from)
    into.erase(
      std::remove_if(
        into.begin(), into.end(),
        [&one](const Fact &held) { return held.stock == one.stock; }),
      into.end());
  into.insert(into.end(), from.begin(), from.end());
}

}  // namespace

void SOUND::HISTORY::folded(Edit &tail, const Edit &fresh) {
  if (!fresh.said.empty()) tail.said = fresh.said;
  if (fresh.verb == Edit::ADDED && tail.verb == Edit::REMOVED) {
    ::joined(tail.gone, tail.placements);
    tail.placements.clear();
    tail.verb = Edit::ADDED;
  }
  ::joined(
    tail.verb == Edit::ADDED && fresh.verb == Edit::REMOVED ? tail.gone
                                                            : tail.placements,
    fresh.placements);
  ::joined(tail.gone, fresh.gone);
  ::joined(tail.swept, fresh.swept);
  ::joined(tail.graph.nodes, fresh.graph.nodes);
  ::joined(tail.graph.wires, fresh.graph.wires);
  ::joined(tail.graph.slack, fresh.graph.slack);
  ::joined(tail.rows, fresh.rows);
  ::joined(tail.lanes, fresh.lanes);
  ::joined(tail.maps, fresh.maps);
  ::joined(tail.changes, fresh.changes);
  ::refreshed(tail.notes, fresh.notes);
  ::refreshed(tail.plots, fresh.plots);
}

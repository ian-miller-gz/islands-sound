// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../graph.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

void placed(const Vector<Placement> &spans) {
  for (const Placement &one : spans)
    TIMELINE::place(
      INVENTORY::held(), one.track, one.lane, one.placement, one.span);
}

void lifted(const Vector<Placement> &spans) {
  for (Whole back = spans.size(); back > 0; --back)
    TIMELINE::lift(
      spans[back - 1].track, spans[back - 1].lane, spans[back - 1].placement);
}

void stood(SHELL::Session &session, const Edit &edit) {
  COMMANDS::rowed(edit, true);
  COMMANDS::stand(session, edit.graph);
  ::placed(edit.placements);
  for (const Mapping &mapped : edit.maps)
    TIMELINE::map(mapped.track, mapped.lane, mapped.map);
}

void dropped(const Edit &edit) {
  for (Whole one = edit.maps.size(); one > 0; --one) {
    const Mapping &mapped = edit.maps[one - 1];
    TIMELINE::map(mapped.track, mapped.lane, mapped.stood);
  }
  ::lifted(edit.placements);
  for (Whole drawn = edit.graph.wires.size(); drawn > 0; --drawn) {
    const Wire &wire = edit.graph.wires[drawn - 1];
    GRAPH::unwire(wire.from, wire.out, wire.to, wire.in);
  }
  for (Whole node = edit.graph.nodes.size(); node > 0; --node)
    COMMANDS::unseated(edit.graph.nodes[node - 1].name);
  COMMANDS::rowed(edit, false);
}

void restated(const Edit &edit, Flag back) {
  const Vector<Placement> &spans = edit.placements;
  for (Whole one = 0; one < spans.size(); ++one) {
    const Placement &at = spans[back ? spans.size() - 1 - one : one];
    TIMELINE::span(at.track, at.lane, at.placement, back ? at.was : at.span);
  }
  const Vector<Change> &changes = edit.changes;
  for (Whole one = 0; one < changes.size(); ++one) {
    const Change &said = changes[back ? changes.size() - 1 - one : one];
    COMMANDS::state(said, back ? said.stood : said.now);
  }
}

void applied(SHELL::Session &session, const Edit &taken, Flag back) {
  COMMANDS::rewrote(taken, back);
  if (taken.verb == Edit::CHANGED) return ::restated(taken, back);
  if (back) {
    if (taken.verb == Edit::ADDED)
      ::dropped(taken);
    else
      ::stood(session, taken);
    return ::placed(taken.gone);
  }
  ::lifted(taken.gone);
  if (taken.verb == Edit::ADDED)
    ::stood(session, taken);
  else
    ::dropped(taken);
}

}  // namespace

auto SOUND::COMMANDS::walked(SHELL::Session &session, Flag back) -> String {
  const Edit *entry = back ? HISTORY::undoable() : HISTORY::redoable();
  if (entry == nullptr)
    return back ? "undo refused: nothing to undo"
                : "redo refused: nothing to redo";
  const Edit taken = *entry;
  ::applied(session, taken, back);
  if (back)
    HISTORY::undone();
  else
    HISTORY::redone();
  return std::format("{} {}", back ? "undone" : "redone", taken.act);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "../history.hpp"
#include "commands.internal.hpp"

void SOUND::COMMANDS::rewired(
  SHELL::Session &session, const Edit &edit, Flag back) {
  const Graph &parted = edit.parted;
  if (back) {
    if (!parted.nodes.empty()) stand(session, {.nodes = parted.nodes});
    for (const Wire &wire : parted.wires)
      GRAPH::hookup(wire.from, wire.out, wire.to, wire.in);
    return;
  }
  for (Whole one = parted.wires.size(); one > 0; --one) {
    const Wire &wire = parted.wires[one - 1];
    GRAPH::unwire(wire.from, wire.out, wire.to, wire.in);
  }
  for (Whole one = parted.nodes.size(); one > 0; --one)
    unseated(parted.nodes[one - 1].name);
}

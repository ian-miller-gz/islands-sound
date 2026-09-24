// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../history.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto mapped(const Edit &edit) -> String {
  return edit.maps.empty() ? String()
                           : std::format(" maps {}", edit.maps.size());
}

auto changed(const Edit &edit) -> String {
  return edit.changes.empty() ? String()
                              : std::format(" changes {}", edit.changes.size());
}

auto rowed(const Edit &edit) -> String {
  return (edit.rows.empty() ? String()
                            : std::format(" tracks {}", edit.rows.size())) +
         (edit.lanes.empty() ? String()
                             : std::format(" lanes {}", edit.lanes.size()));
}

auto written(const Edit &edit) -> String {
  return (edit.notes.empty() ? String()
                             : std::format(" notes {}", edit.notes.size())) +
         (edit.plots.empty() ? String()
                             : std::format(" plots {}", edit.plots.size()));
}

auto said(const Edit &edit) -> String {
  return edit.said.empty() ? String() : std::format(" said {}", edit.said);
}

}  // namespace

auto SOUND::COMMANDS::lined(Whole index, const Edit &edit) -> String {
  return std::format(
    "edit {} {} nodes {} wires {} spans {}{}", index, edit.act,
    edit.graph.nodes.size(), edit.graph.wires.size(), edit.placements.size(),
    ::mapped(edit) + ::changed(edit) + ::rowed(edit) + ::written(edit) +
      ::said(edit));
}

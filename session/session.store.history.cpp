// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "session.store.internal.hpp"
#include "session.store.span.hpp"

using namespace SOUND;

namespace {

auto spanned(STRING::Hot tag, const Vector<Placement> &spans) -> String {
  String text;
  for (const Placement &one : spans) {
    text += std::format(
      "{} {} {} {} {}\n", tag, one.track, one.lane, one.placement,
      SESSION::spanned(one.span));
    text += SESSION::tuned(one.span);
    if (one.was.frames != 0)
      text += std::format("w {}\n", SESSION::spanned(one.was)) +
              SESSION::tuned(one.was);
  }
  return text;
}

auto changed(const Vector<Change> &changes) -> String {
  String text;
  for (const Change &one : changes)
    text += std::format(
      "v {} {}\nn {}\nu {}\n", one.word, one.at, one.now, one.stood);
  return text;
}

auto parted(const Graph &parted) -> String {
  String text;
  for (const Wire &wire : parted.wires)
    text += std::format(
      "xf {}\nxe {} {} {}\n", wire.from, wire.out, wire.in, wire.to);
  return text;
}

auto unseated(const Vector<Node> &nodes) -> String {
  return nodes.empty() ? String()
                       : "xu\n" + SESSION::written(Graph{.nodes = nodes});
}

}  // namespace

auto SOUND::SESSION::written(const History &history, Whole keep) -> String {
  String text;
  const Whole from =
    history.edits.size() - std::min(keep, history.edits.size());
  for (Whole one = from; one < history.edits.size(); ++one) {
    const Edit &edit = history.edits[one];
    text += std::format("x {} {} {}\n", edit.arc, edit.verb, edit.act);
    if (!edit.said.empty()) text += std::format("xs {}\n", edit.said);
    for (const String &name : edit.swept) text += std::format("o {}\n", name);
    text += ::spanned("y", edit.placements) + ::spanned("z", edit.gone);
    for (const Mapping &mapped : edit.maps) {
      text +=
        std::format("m {} {} {}\n", mapped.track, mapped.lane, mapped.map);
      if (!mapped.stood.empty()) text += std::format("d {}\n", mapped.stood);
    }
    text += ::changed(edit.changes) + ::parted(edit.parted) + rowed(edit) +
            content(edit) + written(edit.graph) + ::unseated(edit.parted.nodes);
  }
  return text;
}

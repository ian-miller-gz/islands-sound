// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto spanned(STRING::Hot tag, const Vector<Placement> &spans) -> String {
  String text;
  for (const Placement &one : spans) {
    text += std::format(
      "{} {} {} {} {} {} {} {}\n", tag, one.track, one.lane, one.placement,
      one.span.stock, one.span.at, one.span.from, one.span.frames);
    if (one.was.frames != 0)
      text += std::format(
        "w {} {} {} {}\n", one.was.stock, one.was.at, one.was.from,
        one.was.frames);
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
    text += ::changed(edit.changes) + rowed(edit) + content(edit) +
            written(edit.graph);
  }
  return text;
}

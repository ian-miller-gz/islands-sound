// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto spanned(STRING::Hot tag, const Vector<ARRANGEMENT::TRACK::LANE::Clip> &of)
  -> String {
  String text;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : of)
    text += std::format(
      "{} {} {} {} {}\n", tag, span.stock, span.at, span.from, span.frames);
  return text;
}

auto laned(const ARRANGEMENT::TRACK::Lane &row) -> String {
  String text = std::format("tl {} {}\n", row.kind, row.name);
  if (!row.map.empty()) text += std::format("tm {}\n", row.map);
  return text + ::spanned("tp", row.clips) + ::spanned("to", row.outtakes);
}

auto tracked(const Vector<Row> &rows) -> String {
  String text;
  for (const Row &one : rows) {
    text += std::format("tt {} {}\n", one.track, one.row.name);
    if (one.row.bus) text += "tb\n";
    if (!one.row.root.empty()) text += std::format("tk {}\n", one.row.root);
    for (const ARRANGEMENT::TRACK::Lane &row : one.row.lanes)
      text += ::laned(row);
  }
  return text;
}

auto laning(const Vector<Laning> &lanes) -> String {
  String text;
  for (const Laning &one : lanes)
    text += std::format(
              "tn {} {} {} {} {} {}\n", one.track, one.lane, Whole(one.admits),
              Whole(one.port.bypassed), one.port.kind, one.port.name) +
            ::laned(one.row);
  return text;
}

}  // namespace

auto SOUND::SESSION::rowed(const Edit &edit) -> String {
  return ::tracked(edit.rows) + ::laning(edit.lanes);
}

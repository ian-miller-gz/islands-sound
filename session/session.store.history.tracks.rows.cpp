// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto standing(Edit &edit) -> ARRANGEMENT::TRACK::Lane * {
  if (!edit.lanes.empty()) return &edit.lanes.back().row;
  if (edit.rows.empty() || edit.rows.back().row.lanes.empty()) return nullptr;
  return &edit.rows.back().row.lanes.back();
}

auto clipped(std::istringstream &line) -> ARRANGEMENT::TRACK::LANE::Clip {
  ARRANGEMENT::TRACK::LANE::Clip span;
  line >> span.stock >> span.at >> span.from >> span.frames;
  return span;
}

auto opened(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "tt") {
    Row one;
    line >> one.track;
    one.row.name = SESSION::rest(line);
    edit.rows.push_back(one);
    return true;
  }
  if (tag != "tn") return false;
  Laning one;
  Whole admits = 0, bypassed = 0;
  line >> one.track >> one.lane >> admits >> bypassed >> one.port.kind;
  one.admits = admits != 0;
  one.port.bypassed = bypassed != 0;
  one.port.name = SESSION::rest(line);
  edit.lanes.push_back(one);
  return true;
}

auto hung(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "tb" || tag == "tk") {
    if (edit.rows.empty()) return true;
    if (tag == "tb")
      edit.rows.back().row.bus = true;
    else
      edit.rows.back().row.root = SESSION::rest(line);
    return true;
  }
  if (tag == "tl") {
    if (!edit.lanes.empty()) {
      line >> edit.lanes.back().row.kind;
      edit.lanes.back().row.name = SESSION::rest(line);
      return true;
    }
    if (edit.rows.empty()) return true;
    ARRANGEMENT::TRACK::Lane row;
    line >> row.kind;
    row.name = SESSION::rest(line);
    edit.rows.back().row.lanes.push_back(row);
    return true;
  }
  ARRANGEMENT::TRACK::Lane *row = ::standing(edit);
  if (row == nullptr) return true;
  if (tag == "tm")
    row->map = SESSION::rest(line);
  else if (tag == "tp")
    row->clips.push_back(::clipped(line));
  else
    row->outtakes.push_back(::clipped(line));
  return true;
}

}  // namespace

auto SOUND::SESSION::rowed(
  const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (::opened(tag, line, edit)) return true;
  if (
    tag != "tb" && tag != "tk" && tag != "tl" && tag != "tm" && tag != "tp" &&
    tag != "to")
    return false;
  return ::hung(tag, line, edit);
}

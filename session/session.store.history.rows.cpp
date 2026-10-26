// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"
#include "session.store.span.hpp"

using namespace SOUND;

namespace {

auto spanned(std::istringstream &line) -> Placement {
  Placement one;
  line >> one.track >> one.lane >> one.placement;
  one.span = SESSION::spanned(line);
  return one;
}

auto tuning(Edit &edit) -> ARRANGEMENT::TRACK::LANE::Clip * {
  if (!edit.rows.empty() || !edit.lanes.empty()) return nullptr;
  if (!edit.gone.empty()) return &edit.gone.back().span;
  if (edit.placements.empty()) return nullptr;
  Placement &one = edit.placements.back();
  return one.was.frames != 0 ? &one.was : &one.span;
}

auto placed(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "y")
    edit.placements.push_back(::spanned(line));
  else if (tag == "z")
    edit.gone.push_back(::spanned(line));
  else if (tag == "w" && !edit.placements.empty())
    edit.placements.back().was = SESSION::spanned(line);
  else if (tag == "pt" && ::tuning(edit) != nullptr)
    SESSION::tune(line, *::tuning(edit));
  else
    return false;
  return true;
}

auto headed(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "m") {
    Mapping mapped;
    line >> mapped.track >> mapped.lane;
    mapped.map = SESSION::rest(line);
    edit.maps.push_back(mapped);
    return true;
  }
  if (tag != "v") return false;
  Change said;
  line >> said.word;
  said.at = SESSION::rest(line);
  edit.changes.push_back(said);
  return true;
}

auto worded(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "xs")
    edit.said = SESSION::rest(line);
  else if (tag == "o")
    edit.swept.push_back(SESSION::rest(line));
  else if (tag == "d" && !edit.maps.empty())
    edit.maps.back().stood = SESSION::rest(line);
  else if (tag == "n" && !edit.changes.empty())
    edit.changes.back().now = SESSION::rest(line);
  else if (tag == "u" && !edit.changes.empty())
    edit.changes.back().stood = SESSION::rest(line);
  else
    return false;
  return true;
}

auto parted(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "xf") {
    edit.parted.wires.push_back({.from = SESSION::rest(line)});
    return true;
  }
  if (tag != "xe" || edit.parted.wires.empty()) return false;
  Wire &wire = edit.parted.wires.back();
  line >> wire.out >> wire.in;
  wire.to = SESSION::rest(line);
  return true;
}

}  // namespace

auto SOUND::SESSION::edited(
  const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  return ::placed(tag, line, edit) || ::headed(tag, line, edit) ||
         ::worded(tag, line, edit) || ::parted(tag, line, edit) ||
         rowed(tag, line, edit) || content(tag, line, edit);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto clipped(std::istringstream &line) -> ARRANGEMENT::TRACK::LANE::Clip {
  ARRANGEMENT::TRACK::LANE::Clip span;
  line >> span.stock >> span.at >> span.from >> span.frames;
  return span;
}

auto spanned(std::istringstream &line) -> Placement {
  Placement one;
  line >> one.track >> one.lane >> one.placement;
  one.span = ::clipped(line);
  return one;
}

auto placed(const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "y")
    edit.placements.push_back(::spanned(line));
  else if (tag == "z")
    edit.gone.push_back(::spanned(line));
  else if (tag == "w" && !edit.placements.empty())
    edit.placements.back().was = ::clipped(line);
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

}  // namespace

auto SOUND::SESSION::edited(
  const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  return ::placed(tag, line, edit) || ::headed(tag, line, edit) ||
         ::worded(tag, line, edit) || rowed(tag, line, edit) ||
         content(tag, line, edit);
}

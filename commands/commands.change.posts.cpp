// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <sstream>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto placed(const Change &change) -> Whole {
  return Whole(std::strtoul(change.at.c_str(), nullptr, 10));
}

auto rest(std::istringstream &spoken) -> String {
  String tail;
  std::getline(spoken >> std::ws, tail);
  return tail;
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::flagged(const Change &change) -> String {
  const Whole at = ::placed(change);
  for (const Tag &row : TRANSPORT::held().tags)
    if (row.at == at) return std::format("{} {}", row.at, row.text);
  return String();
}

auto SOUND::COMMANDS::CHANGE::flagged(const Change &change, const String &value)
  -> Flag {
  const Whole at = ::placed(change);
  const Vector<Tag> &tags = TRANSPORT::held().tags;
  if (value.empty()) {
    for (Whole one = 0; one < tags.size(); ++one)
      if (tags[one].at == at) return TRANSPORT::untag(one);
    return false;
  }
  std::istringstream spoken(value);
  Whole place = 0;
  spoken >> place;
  TRANSPORT::tag(place, ::rest(spoken));
  return true;
}

auto SOUND::COMMANDS::CHANGE::ended(const Change &) -> String {
  const Span ends = TRANSPORT::held().ends;
  return ends.to > ends.from ? std::format("{} {}", ends.from, ends.to)
                             : String();
}

auto SOUND::COMMANDS::CHANGE::ended(const Change &, const String &value)
  -> Flag {
  std::istringstream spoken(value);
  Whole from = 0, to = 0;
  spoken >> from >> to;
  TRANSPORT::ends(from, to);
  return true;
}

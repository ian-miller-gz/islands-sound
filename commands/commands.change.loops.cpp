// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <sstream>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto standing(const Change &change) -> Span {
  const Whole at = Whole(std::strtoul(change.at.c_str(), nullptr, 10));
  for (const Span &row : TRANSPORT::held().loops)
    if (row.from == at) return row;
  return {};
}

auto unlooped(Whole at) -> Flag {
  const Vector<Span> &loops = TRANSPORT::held().loops;
  for (Whole one = 0; one < loops.size(); ++one)
    if (loops[one].from == at) return TRANSPORT::unloop(one);
  return false;
}

auto spanned(const String &value) -> Span {
  std::istringstream spoken(value);
  Span row;
  spoken >> row.from >> row.to;
  std::getline(spoken >> std::ws, row.text);
  return row;
}

auto laid(const Span &row) -> Flag {
  const Whole seat = TRANSPORT::loop(row.from, row.to);
  return seat != NONE && TRANSPORT::loop(seat, row.text);
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::looped(const Change &change) -> String {
  const Span row = ::standing(change);
  return row.to > row.from ? std::format("{} {} {}", row.from, row.to, row.text)
                           : String();
}

auto SOUND::COMMANDS::CHANGE::looped(const Change &change, const String &value)
  -> Flag {
  const Span stood = ::standing(change);
  const Flag gone = ::unlooped(stood.from);
  if (value.empty()) return gone;
  if (::laid(::spanned(value))) return true;
  if (gone) ::laid(stood);
  return false;
}

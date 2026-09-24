// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PLACE = "at";
constexpr STRING::Hot RESTING = "off";

void refused(SHELL::Session &session, const String &what) {
  session.print(std::format("metre refused: {}", what));
}

auto split(const String &word, Metre &row) -> Flag {
  const auto slash = word.find('/');
  if (slash == String::npos) return false;
  row.numerator = Whole(std::strtoul(word.c_str(), nullptr, 10));
  row.denominator = Whole(std::strtoul(word.c_str() + slash + 1, nullptr, 10));
  return row.numerator > 0 && row.denominator > 0;
}

void listed(SHELL::Session &session) {
  for (const Metre &row : TRANSPORT::held().metres)
    session.print(
      std::format("metre {}/{} at {}", row.numerator, row.denominator, row.at));
}

}  // namespace

auto SOUND::COMMANDS::asked(const SHELL::Session &session) -> Asked {
  Asked ask{.words = Whole(session.arguments.size())};
  if (ask.words > 2 && session.arguments[ask.words - 2] == ::PLACE) {
    ask.at = Whole(
      std::strtoul(session.arguments[ask.words - 1].c_str(), nullptr, 10));
    ask.placed = true;
    ask.words -= 2;
  }
  ask.off = ask.words > 1 && session.arguments[1] == ::RESTING;
  return ask;
}

void SOUND::COMMANDS::metre(SHELL::Session &session) {
  const Asked ask = asked(session);
  Metre row{ask.at};
  if (ask.off && !metred(ask.at))
    return ::refused(session, std::format("no row at {}", ask.at));
  if (ask.words > 1 && !ask.off) {
    if (!::split(session.arguments[1], row))
      return ::refused(session, session.arguments[1]);
    metred(ask.at, row.numerator, row.denominator);
  }
  ::listed(session);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../timeline.hpp"
#include "commands.offer.hpp"
#include "commands.retouch.hpp"

namespace {
using namespace SOUND;
namespace SHIFT = TIMELINE::SHIFT;

constexpr STRING::Hot HELP = "sets clip shift";
constexpr STRING::Hot USAGE =
  "shift <track> <lane> <placement> <semitones> [cents]";
constexpr STRING::Hot LINE = "shifted {} {}";
constexpr Whole WORDS = 5;
constexpr Integer BASE = 10;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, BASE);
}

auto parsed(const String &token, Integer &value) -> Flag {
  char *end = nullptr;
  value = Integer(std::strtol(token.c_str(), &end, BASE));
  return end != token.c_str();
}

auto interval(const Vector<String> &words, SHIFT::Interval &value) -> Flag {
  if (!::parsed(words[4], value.semitones)) return false;
  return words.size() <= ::WORDS || ::parsed(words[::WORDS], value.cents);
}

void shift(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Whole track = ::counted(words[1]), lane = ::counted(words[2]),
              placement = ::counted(words[3]);
  const Vector<SHIFT::Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size())
    return session.print("shift refused: no placement");
  SHIFT::Interval value;
  if (!::interval(words, value))
    return session.print("shift refused: no interval");
  const SHIFT::Clip now =
    SHIFT::shifted(spans[placement], SHIFT::joined(value));
  if (!COMMANDS::retouch(track, lane, placement, now, SHIFT::WORD))
    return session.print("shift refused");
  session.print(std::format(::LINE, placement, now.cents));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{SHIFT::WORD, ::HELP, ::shift}});

}  // namespace

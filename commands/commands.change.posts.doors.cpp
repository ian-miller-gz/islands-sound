// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RESTING = "off";

auto worded(STRING::Hot word, const String &where, const String &text)
  -> String {
  return text.empty() ? std::format("{} {}", word, where)
                      : std::format("{} {} {}", word, where, text);
}

auto phrased(STRING::Hot word, Whole at) -> String {
  return std::format("{} {} at {}", word, ::RESTING, at);
}

}  // namespace

auto SOUND::COMMANDS::flagged(Whole at, const String &text) -> Flag {
  return kept(
    {.word = HISTORY::WORD::TAG,
     .at = std::format("{}", at),
     .now = std::format("{} {}", at, text)},
    ::worded(HISTORY::WORD::TAG, std::format("{}", at), text));
}

auto SOUND::COMMANDS::flagged(Whole at) -> Flag {
  return kept(
    {.word = HISTORY::WORD::TAG, .at = std::format("{}", at)},
    ::phrased(HISTORY::WORD::TAG, at));
}

auto SOUND::COMMANDS::looped(Whole from, Whole to, const String &text) -> Flag {
  return kept(
    {.word = HISTORY::WORD::LOOP,
     .at = std::format("{}", from),
     .now = std::format("{} {} {}", from, to, text)},
    ::worded(HISTORY::WORD::LOOP, std::format("{} {}", from, to), text));
}

auto SOUND::COMMANDS::looped(Whole from) -> Flag {
  return kept(
    {.word = HISTORY::WORD::LOOP, .at = std::format("{}", from)},
    ::phrased(HISTORY::WORD::LOOP, from));
}

auto SOUND::COMMANDS::ended(Whole from, Whole to) -> Flag {
  const String bounds = to > from ? std::format("{} {}", from, to) : String();
  return kept(
    {.word = HISTORY::WORD::ENDS, .now = bounds},
    std::format(
      "{} {}", HISTORY::WORD::ENDS,
      bounds.empty() ? String(::RESTING) : bounds));
}

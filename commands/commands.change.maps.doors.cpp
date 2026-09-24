// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../score.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RESTING = "off";

auto phrased(STRING::Hot word, const String &value, Whole at) -> String {
  return at == 0 ? std::format("{} {}", word, value)
                 : std::format("{} {} at {}", word, value, at);
}

}  // namespace

auto SOUND::COMMANDS::paced(Whole at, Float tempo) -> Flag {
  const String value = std::format("{:g}", tempo);
  return kept(
    {.word = HISTORY::WORD::TEMPO, .at = std::format("{}", at), .now = value},
    ::phrased(HISTORY::WORD::TEMPO, value, at));
}

auto SOUND::COMMANDS::paced(Whole at) -> Flag {
  return kept(
    {.word = HISTORY::WORD::TEMPO, .at = std::format("{}", at)},
    ::phrased(HISTORY::WORD::TEMPO, String(::RESTING), at));
}

auto SOUND::COMMANDS::metred(Whole at, Whole numerator, Whole denominator)
  -> Flag {
  const String value = std::format("{}/{}", numerator, denominator);
  return kept(
    {.word = HISTORY::WORD::METRE, .at = std::format("{}", at), .now = value},
    ::phrased(HISTORY::WORD::METRE, value, at));
}

auto SOUND::COMMANDS::metred(Whole at) -> Flag {
  return kept(
    {.word = HISTORY::WORD::METRE, .at = std::format("{}", at)},
    ::phrased(HISTORY::WORD::METRE, String(::RESTING), at));
}

auto SOUND::COMMANDS::keyed(Whole at, Whole tonic, const String &scale)
  -> Flag {
  return kept(
    {.word = HISTORY::WORD::SCALE,
     .at = std::format("{}", at),
     .now = std::format("{} {}", tonic, scale)},
    ::phrased(
      HISTORY::WORD::SCALE, std::format("{} {}", SCORE::classed(tonic), scale),
      at));
}

auto SOUND::COMMANDS::keyed(Whole at) -> Flag {
  return kept(
    {.word = HISTORY::WORD::SCALE, .at = std::format("{}", at)},
    ::phrased(HISTORY::WORD::SCALE, String(::RESTING), at));
}

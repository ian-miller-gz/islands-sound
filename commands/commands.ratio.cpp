// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <cstdlib>
#include <format>

#include "../timeline.hpp"
#include "commands.offer.hpp"
#include "commands.retouch.hpp"

namespace {
using namespace SOUND;
namespace RATIO = TIMELINE::RATIO;

constexpr STRING::Hot HELP = "sets clip ratio";
constexpr STRING::Hot USAGE = "ratio <track> <lane> <placement> <percent>";
constexpr STRING::Hot LINE = "ratioed {} {:g}";
constexpr Whole WORDS = 5;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto percent(const String &token, Float &value) -> Flag {
  char *end = nullptr;
  value = std::strtof(token.c_str(), &end);
  return end != token.c_str() && std::isfinite(value);
}

void ratio(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Whole track = ::counted(words[1]), lane = ::counted(words[2]),
              placement = ::counted(words[3]);
  const Vector<RATIO::Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size())
    return session.print("ratio refused: no placement");
  Float value = 0.0f;
  if (!::percent(words[4], value))
    return session.print("ratio refused: no percent");
  const RATIO::Clip now = RATIO::stretched(spans[placement], value);
  if (!COMMANDS::retouch(track, lane, placement, now, RATIO::WORD))
    return session.print("ratio refused");
  session.print(std::format(::LINE, placement, now.ratio * RATIO::PERCENT));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{RATIO::WORD, ::HELP, ::ratio}});

}  // namespace

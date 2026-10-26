// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../fade.hpp"
#include "commands.retouch.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;

struct Tail {
  Flag (*stands)(const Clip &);
  String (*said)(const Clip &);
};

constexpr Tail TAILS[] = {
  {[](const Clip &span) -> Flag {
     return span.in.frames != 0 || span.out.frames != 0;
   },
   [](const Clip &span) -> String {
     return std::format(
       " fade {} {} {} {}", span.in.frames, FADE::shaped(span.in.shape).word,
       span.out.frames, FADE::shaped(span.out.shape).word);
   }},
  {[](const Clip &span) -> Flag { return span.ratio != Clip().ratio; },
   [](const Clip &span) -> String {
     return std::format(" ratio {:g}", span.ratio);
   }},
  {[](const Clip &span) -> Flag { return span.cents != Clip().cents; },
   [](const Clip &span) -> String {
     return std::format(" cents {}", span.cents);
   }},
  {[](const Clip &span) -> Flag { return !span.tunes.empty(); },
   [](const Clip &span) -> String {
     return std::format(" tunes {}", span.tunes.size());
   }},
};

}  // namespace

auto SOUND::COMMANDS::tailed(const Clip &span) -> String {
  String text;
  for (const ::Tail &tail : ::TAILS)
    if (tail.stands(span)) text += tail.said(span);
  return text;
}

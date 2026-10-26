// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "../commands.hpp"
#include "../timeline.hpp"
#include "commands.offer.hpp"
#include "commands.retouch.hpp"
#include "commands.tune.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
namespace TUNE = COMMANDS::TUNE;

constexpr STRING::Hot UNTUNE = "untune";
constexpr STRING::Hot HELP = "tunes clip notes", CLEARS = "clears clip tunes";
constexpr STRING::Hot USAGE = "tune <track> <lane> <placement> [strength]";
constexpr STRING::Hot UNUSAGE = "untune <track> <lane> <placement>";
constexpr STRING::Hot LINE = "tuned {} tunes {}";
constexpr STRING::Hot CLEARED = "untuned {}";
constexpr Whole WORDS = 4;
constexpr Integer BASE = 10;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

void tune(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Float strength = words.size() > ::WORDS
                           ? std::strtof(words[::WORDS].c_str(), nullptr)
                           : TIMELINE::TUNE::STRENGTH;
  const Whole placement = ::counted(words[3]);
  const Whole tunes = COMMANDS::tuned(
    ::counted(words[1]), ::counted(words[2]), placement, strength);
  if (tunes == NONE) return session.print("tune refused: no placement");
  session.print(std::format(::LINE, placement, tunes));
}

void untune(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::UNUSAGE);
  const Whole placement = ::counted(words[3]);
  if (!COMMANDS::untuned(::counted(words[1]), ::counted(words[2]), placement))
    return session.print("untune refused: no tunes");
  session.print(std::format(::CLEARED, placement));
}

[[maybe_unused]] const Flag offered = COMMANDS::offer(
  {{TIMELINE::TUNE::WORD, ::HELP, ::tune}, {::UNTUNE, ::CLEARS, ::untune}});

}  // namespace

auto SOUND::COMMANDS::tuned(
  Whole track, Whole lane, Whole placement, Float strength) -> Whole {
  const Vector<Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size()) return NONE;
  Clip now = spans[placement];
  now.tunes.clear();
  now.tunes = TUNE::tunes(
    now, std::clamp(strength, TIMELINE::TUNE::LEAST, TIMELINE::TUNE::MOST));
  if (!retouch(track, lane, placement, now, TIMELINE::TUNE::WORD)) return NONE;
  return now.tunes.size();
}

auto SOUND::COMMANDS::untuned(Whole track, Whole lane, Whole placement)
  -> Flag {
  const Vector<Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size() || spans[placement].tunes.empty()) return false;
  Clip now = spans[placement];
  now.tunes.clear();
  return retouch(track, lane, placement, now, ::UNTUNE);
}

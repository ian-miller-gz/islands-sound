// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <format>

#include "../onset.hpp"
#include "../timeline.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;

constexpr STRING::Hot WORD = "onsets";
constexpr STRING::Hot HELP = "lists clip onsets";
constexpr STRING::Hot USAGE = "onsets <track> <lane> <placement> [sense]";
constexpr STRING::Hot COUNT = "onsets {}";
constexpr STRING::Hot LINE = "onset {} at {}";
constexpr Whole WORDS = 4;
constexpr Integer BASE = 10;
constexpr Float LEAST = 0.0f, MOST = 1.0f;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

auto sensed(const Vector<String> &words, Float &value) -> Flag {
  if (words.size() <= ::WORDS) return true;
  char *end = nullptr;
  value = std::strtof(words[::WORDS].c_str(), &end);
  if (end == words[::WORDS].c_str() || !std::isfinite(value)) return false;
  value = std::clamp(value, ::LEAST, ::MOST);
  return true;
}

auto found(const Clip &clip, Float sense) -> Vector<Whole> {
  const TIMELINE::Derived source = TIMELINE::derived(clip.stock, clip);
  if (source.take == nullptr) return {};
  return TIMELINE::placed(clip, ONSET::found(*source.take, sense));
}

void onsets(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Vector<Clip> spans =
    TIMELINE::spans(::counted(words[1]), ::counted(words[2]));
  const Whole placement = ::counted(words[3]);
  if (placement >= spans.size())
    return session.print("onsets refused: no placement");
  Float sense = ONSET::RESTING;
  if (!::sensed(words, sense)) return session.print("onsets refused: no sense");
  const Vector<Whole> frames = ::found(spans[placement], sense);
  session.print(std::format(::COUNT, frames.size()));
  for (Whole at = 0; at < frames.size(); ++at)
    session.print(std::format(::LINE, at, frames[at]));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::WORD, ::HELP, ::onsets}});

}  // namespace

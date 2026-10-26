// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../contour.hpp"
#include "../timeline.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;

constexpr STRING::Hot WORD = "contour";
constexpr STRING::Hot HELP = "lists clip notes";
constexpr STRING::Hot USAGE = "contour <track> <lane> <placement>";
constexpr STRING::Hot COUNT = "contour {}";
constexpr STRING::Hot LINE = "found {} at {} for {} pitch {} cents {}";
constexpr Whole WORDS = 4;
constexpr Integer BASE = 10;

struct Laid {
  Whole at = 0;
  CONTOUR::Found found;
};

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

auto laid(const Clip &clip) -> Vector<Laid> {
  const TIMELINE::Derived source = TIMELINE::derived(clip.stock, clip);
  if (source.take == nullptr) return {};
  Vector<Laid> notes;
  for (const CONTOUR::Found &found :
       CONTOUR::noted(CONTOUR::traced(*source.take)))
    for (Whole at : TIMELINE::placed(clip, {found.at}))
      notes.push_back({at, found});
  return notes;
}

void contour(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Vector<Clip> spans =
    TIMELINE::spans(::counted(words[1]), ::counted(words[2]));
  const Whole placement = ::counted(words[3]);
  if (placement >= spans.size())
    return session.print("contour refused: no placement");
  const Vector<Laid> notes = ::laid(spans[placement]);
  session.print(std::format(::COUNT, notes.size()));
  for (Whole at = 0; at < notes.size(); ++at) {
    const CONTOUR::Found &found = notes[at].found;
    session.print(std::format(
      ::LINE, at, notes[at].at, found.frames, found.pitch, found.cents));
  }
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::WORD, ::HELP, ::contour}});

}  // namespace

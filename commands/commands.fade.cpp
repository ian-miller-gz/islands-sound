// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <iterator>

#include "../fade.hpp"
#include "../timeline.hpp"
#include "commands.offer.hpp"
#include "commands.retouch.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
using ARRANGEMENT::TRACK::LANE::Fade;

constexpr STRING::Hot HELP = "sets clip fade";
constexpr STRING::Hot USAGE =
  "fade <track> <lane> <placement> in|out <frames> [shape]";
constexpr STRING::Hot LINE = "faded {} {} {} {}";
constexpr Whole SHAPED = 6;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto ended(const String &word) -> const FADE::End * {
  for (const FADE::End &end : FADE::ENDS)
    if (word == end.word) return &end;
  return nullptr;
}

auto shape(const String &word) -> Whole {
  for (Whole row = 0; row < std::size(FADE::SHAPES); ++row)
    if (word == FADE::SHAPES[row].word) return row;
  return NONE;
}

auto faded(Clip span, const FADE::End &end, Whole frames, Whole shape) -> Clip {
  Fade &fade = span.*end.fade;
  if (frames == 0) {
    fade = {};
    return span;
  }
  fade = {FADE::held(Integer(frames), FADE::room(span, end)), shape};
  return span;
}

void fade(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::SHAPED) return session.print(::USAGE);
  const Whole track = ::counted(words[1]), lane = ::counted(words[2]),
              placement = ::counted(words[3]);
  const Vector<Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size())
    return session.print("fade refused: no placement");
  const FADE::End *end = ::ended(words[4]);
  if (end == nullptr) return session.print("fade refused: no end");
  const Whole shape = words.size() > ::SHAPED
                        ? ::shape(words[::SHAPED])
                        : (spans[placement].*end->fade).shape;
  if (shape == NONE) return session.print("fade refused: no shape");
  const Clip now = ::faded(spans[placement], *end, ::counted(words[5]), shape);
  if (!COMMANDS::retouch(track, lane, placement, now, FADE::WORD))
    return session.print("fade refused");
  const Fade &laid = now.*end->fade;
  session.print(std::format(
    ::LINE, placement, end->word, laid.frames, FADE::shaped(laid.shape).word));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{FADE::WORD, ::HELP, ::fade}});

}  // namespace

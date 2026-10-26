// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../commands.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "commands.offer.hpp"
#include "commands.split.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;

constexpr STRING::Hot COMMAND = "split";
constexpr STRING::Hot HELP = "splits clip, states last split";
constexpr STRING::Hot USAGE = "split <track> <lane> <placement> <at>";
constexpr STRING::Hot LINE = "split track {} lane {} placement {} at {}";
constexpr STRING::Hot REFUSED = "split refused: outside clip";
constexpr Whole ARITY = 5;
constexpr Integer BASE = 10;

struct Cut {
  Whole track = NONE, lane = NONE, placement = NONE, at = NONE;
};

::Cut last;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

void split(SHELL::Session &session) {
  const Vector<String> &given = session.arguments;
  if (given.size() < ::ARITY && ::last.track == NONE)
    return session.print(::USAGE);
  if (
    given.size() >= ::ARITY &&
    COMMANDS::split(
      ::counted(given[1]), ::counted(given[2]), ::counted(given[3]),
      ::counted(given[4])) == NONE)
    return session.print(::REFUSED);
  session.print(std::format(
    ::LINE, ::last.track, ::last.lane, ::last.placement, ::last.at));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::COMMAND, ::HELP, ::split}});

}  // namespace

auto SOUND::COMMANDS::split(Whole track, Whole lane, Whole placement, Whole at)
  -> Whole {
  const Vector<Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size()) return NONE;
  const Clip span = spans[placement];
  if (at <= span.at || at >= span.at + span.frames) return NONE;
  const Whole kind = TIMELINE::held().tracks[track].lanes[lane].kind;
  const Vector<Clip> pieces = SPLIT::cut(span, kind, at);
  if (!TIMELINE::lift(track, lane, placement)) return NONE;
  Edit edit = {.act = ::COMMAND, .gone = {{track, lane, placement, span}}};
  for (Whole piece = 0; piece < pieces.size(); ++piece) {
    const Whole row = placement + piece;
    TIMELINE::place(INVENTORY::held(), track, lane, row, pieces[piece]);
    edit.placements.push_back({track, lane, row, pieces[piece]});
  }
  HISTORY::record(edit);
  ::last = {track, lane, placement, at};
  return placement + 1;
}

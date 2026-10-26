// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../commands.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../onset.hpp"
#include "../timeline.hpp"
#include "../views.hpp"
#include "commands.offer.hpp"
#include "commands.quantize.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;

constexpr STRING::Hot WORD = "quantize";
constexpr STRING::Hot HELP = "snaps clip onsets";
constexpr STRING::Hot USAGE = "quantize <track> <lane> <placement> [grid]";
constexpr STRING::Hot LINE = "quantized pieces {}";
constexpr Whole WORDS = 4;
constexpr Integer BASE = 10;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

void stand(Whole track, Whole lane, Whole placement, const Vector<Clip> &cut) {
  Vector<Placement> stood;
  for (const Clip &one : cut) {
    const Whole at = placement + stood.size();
    TIMELINE::place(INVENTORY::held(), track, lane, at, one);
    stood.push_back({track, lane, at, one});
  }
  HISTORY::record({.act = ::WORD, .placements = stood});
}

void quantize(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() < ::WORDS) return session.print(::USAGE);
  const Whole track = ::counted(words[1]), lane = ::counted(words[2]);
  const Whole placement = ::counted(words[3]);
  if (placement >= TIMELINE::spans(track, lane).size())
    return session.print("quantize refused: no placement");
  const Whole grid =
    words.size() > ::WORDS ? ::counted(words[::WORDS]) : VIEWS::GET::grid();
  if (grid == 0) return session.print("quantize refused: no grid");
  const Whole pieces =
    COMMANDS::quantized(track, lane, placement, grid, ONSET::RESTING);
  if (pieces == NONE) return session.print("quantize refused: no onsets");
  session.print(std::format(::LINE, pieces));
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::WORD, ::HELP, ::quantize}});

}  // namespace

auto SOUND::COMMANDS::quantized(
  Whole track, Whole lane, Whole placement, Whole grid, Float sense) -> Whole {
  const Vector<Clip> spans = TIMELINE::spans(track, lane);
  if (placement >= spans.size()) return NONE;
  const Clip clip = spans[placement];
  const Vector<Clip> pieces = QUANTIZE::cut(clip, grid, sense);
  if (pieces.empty()) return NONE;
  HISTORY::begin();
  TIMELINE::lift(track, lane, placement);
  HISTORY::record(
    {.act = ::WORD,
     .verb = Edit::REMOVED,
     .placements = {{track, lane, placement, clip}}});
  ::stand(track, lane, placement, pieces);
  HISTORY::end();
  return pieces.size();
}

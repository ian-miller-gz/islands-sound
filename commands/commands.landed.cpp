// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../inventory.hpp"
#include "../score.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto written(const CONTROL::Take &take) -> Score {
  Score score;
  for (const CONTROL::Played &note : take.notes)
    SCORE::write(
      score, {.pitch = note.pitch,
              .at = SCORE::pulsed(note.at, take.tempo),
              .length = SCORE::pulsed(note.length, take.tempo),
              .velocity = note.velocity});
  return score;
}

auto seat(const CONTROL::Take &take) -> Whole {
  const Whole track = CONTROL::driven();
  if (take.notes.empty() || track >= TIMELINE::held().tracks.size())
    return NONE;
  const Whole lane = COMMANDS::landing(track, KIND::NOTES);
  if (lane == NONE) return NONE;
  const Whole stock = INVENTORY::add(
    std::format("take-{}", INVENTORY::held().stocks.size()), KIND::NOTES);
  if (stock == NONE) return NONE;
  INVENTORY::score(stock, ::written(take));
  COMMANDS::covered(track, lane, take.at, take.at + take.frames);
  return TIMELINE::place(
    INVENTORY::held(), track, lane,
    {.stock = stock, .at = take.at, .from = 0, .frames = take.frames});
}

auto same(const Address &one, const Address &two) -> Flag {
  return one.node == two.node && one.parameter == two.parameter;
}

auto met(const Vector<Address> &seen, const Address &address) -> Flag {
  for (const Address &held : seen)
    if (::same(held, address)) return true;
  return false;
}

void turn(const CONTROL::Take &take) {
  const Whole track = CONTROL::driven();
  if (take.turned.empty() || track >= TIMELINE::held().tracks.size()) return;
  const Whole lane = COMMANDS::landing(track, KIND::LOGIC);
  if (lane == NONE) return;
  COMMANDS::covered(track, lane, take.at, take.at + take.frames);
  Vector<Address> seen;
  for (const CONTROL::Turned &first : take.turned) {
    if (::met(seen, first.address)) continue;
    seen.push_back(first.address);
    const Whole stock = INVENTORY::add(
      std::format("take-{}", INVENTORY::held().stocks.size()), KIND::LOGIC);
    if (stock == NONE) continue;
    INVENTORY::aim(stock, first.address);
    for (const CONTROL::Turned &move : take.turned)
      if (::same(move.address, first.address))
        INVENTORY::plot(stock, {.at = move.at, .value = move.value});
    TIMELINE::place(
      INVENTORY::held(), track, lane,
      {.stock = stock, .at = take.at, .from = 0, .frames = take.frames});
  }
}

}  // namespace

auto SOUND::COMMANDS::landing(Whole track, Whole kind) -> Whole {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  for (Whole at = 0; at < lanes.size(); ++at)
    if (lanes[at].kind == kind) return at;
  return laned(track, kind);
}

void SOUND::COMMANDS::covered(
  Whole track, Whole lane, Whole opens, Whole closes) {
  Vector<ARRANGEMENT::TRACK::LANE::Clip> moved, deleted;
  TIMELINE::bump(track, lane, opens, closes, moved, deleted);
}

void SOUND::COMMANDS::land() {
  const CONTROL::Take &take = CONTROL::landed();
  ::seat(take);
  landed(take);
  ::turn(take);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../inventory.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAMED = "take-{}";

auto armed(Whole track) -> Whole {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  const Whole lane = CONTROL::laned();
  if (lane >= lanes.size() || lanes[lane].kind != KIND::CONTROL) return NONE;
  return lane;
}

}  // namespace

void SOUND::COMMANDS::dialled(const CONTROL::Take &take) {
  const Whole track = CONTROL::driven();
  if (take.dialled.empty() || track >= TIMELINE::held().tracks.size()) return;
  const Whole lane = ::armed(track);
  if (lane == NONE) return;
  const Whole stock = INVENTORY::add(
    std::format(::NAMED, INVENTORY::held().stocks.size()), KIND::CONTROL);
  if (stock == NONE) return;
  for (const CONTROL::Dialled &move : take.dialled)
    INVENTORY::turn(
      stock, {.at = move.at, .number = move.number, .value = move.value});
  INVENTORY::length(stock, take.frames);
  COMMANDS::covered(track, lane, take.at, take.at + take.frames);
  TIMELINE::place(
    INVENTORY::held(), track, lane,
    {.stock = stock, .at = take.at, .from = 0, .frames = take.frames});
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ACT = "map";

}  // namespace

auto SOUND::VIEWS::SIGNALS::edited() -> const ARRANGEMENT::TRACK::Lane * {
  const Whole track = attended(), lane = laned();
  if (track == NONE || lane == NONE) return nullptr;
  return &TIMELINE::held().tracks[track].lanes[lane];
}

void SOUND::VIEWS::SIGNALS::map(const String &plugin) {
  const ARRANGEMENT::TRACK::Lane *held = edited();
  if (held == nullptr) return;
  const Whole track = attended(), lane = laned();
  const String stood = held->map;
  if (!TIMELINE::map(track, lane, plugin)) return;
  HISTORY::record(
    {.act = ::ACT,
     .maps = {{.track = track, .lane = lane, .map = plugin, .stood = stood}}});
}

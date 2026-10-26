// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../views.aim.hpp"
#include "signals.mapping.hpp"

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

void SOUND::VIEWS::SIGNALS::aim(const Address &address) {
  const Inventory &pool = INVENTORY::held();
  HISTORY::begin();
  for (const Span &span : spans())
    if (
      span.stock < pool.stocks.size() &&
      pool.stocks[span.stock].kind == KIND::LOGIC)
      VIEWS::aimed(span.stock, address.node, address.parameter);
  HISTORY::end();
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../inventory.hpp"
#include "peaks.hpp"

namespace {
using namespace SOUND;
using PEAKS::Level;

struct Kept {
  Level level;
  Whole stirred = NONE;
};

Vector<Kept> kept;
const Level blank;

}  // namespace

auto SOUND::PEAKS::GET::level(Whole stock) -> const Level & {
  const Inventory &pool = INVENTORY::held();
  if (stock >= pool.stocks.size() || pool.stocks[stock].kind != KIND::AUDIO)
    return ::blank;
  if (stock >= ::kept.size()) ::kept.resize(stock + 1);
  Kept &held = ::kept[stock];
  const Whole stirred = INVENTORY::stirred(stock);
  if (held.stirred != stirred) {
    held.level = PEAKS::level(pool.stocks[stock].take);
    held.stirred = stirred;
  }
  return held.level;
}

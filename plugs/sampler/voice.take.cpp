// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float SECOND = 1000.0f;

}  // namespace

void SOUND::SAMPLER::stock(Player &player) {
  const Inventory &pool = INVENTORY::held();
  player.ratio = Float(RATE) / Float(player.rate);
  for (const Stock &stock : pool.stocks) {
    if (stock.kind != KIND::AUDIO || stock.take.lanes.empty()) continue;
    player.sources.push_back({.name = stock.name, .lanes = stock.take.lanes});
  }
}

auto SOUND::SAMPLER::frames(const Source &source) -> Whole {
  return source.lanes.empty() ? 0 : source.lanes[0].size();
}

auto SOUND::SAMPLER::span(const Player &player) -> Float {
  if (player.chosen >= player.sources.size()) return 0;
  return Float(frames(player.sources[player.chosen])) * ::SECOND / Float(RATE);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../inventory.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;

constexpr Float SOFTEST = 0.0f;

auto counted() -> Vector<Whole> {
  Vector<Whole> counts(VIEWS::ROLL::TIERS, 0);
  for (const VIEWS::ROLL::Pip &pip : VIEWS::ROLL::plates())
    if (const Whole tier = VIEWS::ROLL::tier(pip); tier != NONE) ++counts[tier];
  return counts;
}

}  // namespace

auto SOUND::VIEWS::ROLL::tier(const Pip &pip) -> Whole {
  const Inventory &pool = INVENTORY::held();
  if (pip.stock >= pool.stocks.size()) return NONE;
  const Score &score = pool.stocks[pip.stock].score;
  if (pip.index >= score.notes.size()) return NONE;
  const Float scaled = score.notes[pip.index].velocity * Float(TIERS);
  return std::min(Whole(std::max(scaled, ::SOFTEST)), TIERS - 1);
}

auto SOUND::VIEWS::ROLL::tinted(STRING::Hot stem, Whole tier) -> String {
  return std::format("{}{}", stem, tier);
}

void SOUND::VIEWS::ROLL::tinted(SHELL::Session &session) {
  String line = "tints";
  for (const Whole count : ::counted()) line += std::format(" {}", count);
  session.print(line);
}

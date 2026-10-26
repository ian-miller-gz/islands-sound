// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "piano.loudness.hpp"

namespace {
using namespace SOUND;
namespace LOUDNESS = SOUND::VIEWS::ROLL::LOUDNESS;

constexpr STRING::Hot ACT = "loudness";

void wrote(const LOUDNESS::Hold &hold, Whole stock) {
  Score score = INVENTORY::held().stocks[stock].score;
  Flag moved = false;
  for (const LOUDNESS::Touch &one : hold.touched) {
    if (one.stock != stock || one.index >= score.notes.size()) continue;
    if (score.notes[one.index].velocity == one.velocity) continue;
    score.notes[one.index].velocity = one.velocity;
    moved = true;
  }
  if (!moved) return;
  HISTORY::take(stock, INVENTORY::held().stocks[stock]);
  INVENTORY::score(stock, score);
  HISTORY::wrote(::ACT, stock, INVENTORY::held().stocks[stock]);
}

}  // namespace

auto SOUND::VIEWS::ROLL::LOUDNESS::clamped(Float velocity) -> Float {
  return std::clamp(velocity, SOFTEST, LOUDEST);
}

auto SOUND::VIEWS::ROLL::LOUDNESS::scored(Whole stock, Whole index) -> Float {
  const Inventory &pool = INVENTORY::held();
  if (stock >= pool.stocks.size()) return SOFTEST;
  const Score &score = pool.stocks[stock].score;
  return index < score.notes.size() ? score.notes[index].velocity : SOFTEST;
}

auto SOUND::VIEWS::ROLL::LOUDNESS::touch(Hold &hold, Whole stock, Whole index)
  -> Touch & {
  for (Touch &one : hold.touched)
    if (one.stock == stock && one.index == index) return one;
  const Float now = scored(stock, index);
  hold.touched.push_back({stock, index, now, now});
  return hold.touched.back();
}

void SOUND::VIEWS::ROLL::LOUDNESS::wrote(const Hold &hold) {
  Vector<Whole> stocks;
  for (const Touch &one : hold.touched)
    if (std::ranges::find(stocks, one.stock) == stocks.end())
      stocks.push_back(one.stock);
  for (const Whole stock : stocks)
    if (stock < INVENTORY::held().stocks.size()) ::wrote(hold, stock);
}

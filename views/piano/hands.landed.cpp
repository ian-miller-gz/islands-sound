// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "piano.acts.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

struct Fall {
  Whole stock = NONE;
  Vector<SCORE::Landing> landings;
};

auto grouped(Vector<::Fall> &falls, Whole stock) -> Vector<SCORE::Landing> & {
  for (::Fall &fall : falls)
    if (fall.stock == stock) return fall.landings;
  falls.push_back({stock, {}});
  return falls.back().landings;
}

auto noted(const ROLL::Pip &pip) -> const Note * {
  const Inventory &pool = INVENTORY::held();
  if (pip.stock >= pool.stocks.size()) return nullptr;
  const Score &score = pool.stocks[pip.stock].score;
  return pip.index < score.notes.size() ? &score.notes[pip.index] : nullptr;
}

void wrote(const ::Fall &fall, STRING::Hot act, Vector<ROLL::Pip> &following) {
  Score score = INVENTORY::held().stocks[fall.stock].score;
  const Vector<Whole> landed = SCORE::land(score, fall.landings);
  HISTORY::take(fall.stock, INVENTORY::held().stocks[fall.stock]);
  INVENTORY::score(fall.stock, score);
  HISTORY::wrote(act, fall.stock, INVENTORY::held().stocks[fall.stock]);
  for (const Whole index : landed)
    if (index != NONE) following.push_back({fall.stock, index});
}

}  // namespace

auto SOUND::VIEWS::ROLL::seated(GUI::Handle page) -> Vector<Pip> {
  const Vector<Pip> &drawn = plates();
  Vector<Pip> pips;
  for (const Whole row : chosen(page))
    if (row < drawn.size() && ::noted(drawn[row]) != nullptr)
      pips.push_back(drawn[row]);
  return pips;
}

void SOUND::VIEWS::ROLL::landed(
  const Vector<Landing> &landings, STRING::Hot act) {
  Vector<::Fall> falls;
  for (const Landing &one : landings) {
    const Note *note = ::noted(one.pip);
    if (note == nullptr) continue;
    const Integer at =
      Integer(note->at) + Integer(one.at) - Integer(one.pip.at);
    if (at < 0) continue;
    ::grouped(falls, one.pip.stock)
      .push_back({one.pip.index, Whole(at), one.pitch});
  }
  if (falls.empty()) return;
  Vector<Pip> following;
  HISTORY::begin();
  for (const ::Fall &fall : falls) ::wrote(fall, act, following);
  HISTORY::end();
  follow(following);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../../score.hpp"
#include "piano.acts.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot DOUBLED = "duplicate";

struct Copy {
  Note note;
  Whole at = 0, track = NONE;
};

struct Fall {
  Whole stock = NONE;
  Score score;
  Vector<SCORE::Landing> landings;
};

auto spanned(const Vector<ROLL::Pip> &pips) -> Whole {
  Whole opens = pips.front().at, closes = 0;
  for (const ROLL::Pip &pip : pips) {
    opens = std::min(opens, pip.at);
    closes = std::max(closes, pip.at + pip.length);
  }
  const Whole grain = ROLL::grained();
  return (closes - opens + grain - 1) / grain * grain;
}

auto copies(const Vector<ROLL::Pip> &pips) -> Vector<::Copy> {
  const Whole span = ::spanned(pips);
  Vector<::Copy> made;
  for (const ROLL::Pip &pip : pips)
    made.push_back(
      {INVENTORY::held().stocks[pip.stock].score.notes[pip.index],
       pip.at + span, pip.track});
  return made;
}

auto claimed(const ::Copy &copy) -> ROLL::Aim {
  return copy.track == NONE ? ROLL::claimed(copy.at)
                            : ROLL::claimed(copy.track, copy.at);
}

auto grouped(Vector<::Fall> &falls, Whole stock) -> ::Fall & {
  for (::Fall &fall : falls)
    if (fall.stock == stock) return fall;
  falls.push_back({stock, INVENTORY::held().stocks[stock].score, {}});
  return falls.back();
}

void written(Vector<::Fall> &falls, const ::Copy &copy) {
  const ROLL::Aim aim = ::claimed(copy);
  if (aim.stock == NONE) return;
  ::Fall &filed = ::grouped(falls, aim.stock);
  Note note = copy.note;
  note.at = ROLL::inside(aim, copy.at);
  const Whole index = SCORE::write(filed.score, note);
  for (SCORE::Landing &prior : filed.landings)
    if (prior.index >= index) ++prior.index;
  filed.landings.push_back({index, note.at, note.pitch});
}

void laid(::Fall &fall, Vector<ROLL::Pip> &following) {
  const Vector<Whole> stood = SCORE::land(fall.score, fall.landings);
  HISTORY::take(fall.stock, INVENTORY::held().stocks[fall.stock]);
  INVENTORY::score(fall.stock, fall.score);
  HISTORY::wrote(::DOUBLED, fall.stock, INVENTORY::held().stocks[fall.stock]);
  for (const Whole index : stood)
    if (index != NONE) following.push_back({fall.stock, index});
}

}  // namespace

void SOUND::VIEWS::ROLL::doubled(GUI::Handle page) {
  const Vector<Pip> pips = seated(page);
  if (pips.empty()) return;
  const Vector<::Copy> made = ::copies(pips);
  for (const ::Copy &copy : made) ::claimed(copy);
  Vector<::Fall> falls;
  for (const ::Copy &copy : made) ::written(falls, copy);
  Vector<Pip> following;
  HISTORY::begin();
  for (::Fall &fall : falls) ::laid(fall, following);
  HISTORY::end();
  follow(following);
}

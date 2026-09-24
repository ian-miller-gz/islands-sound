// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <functional>

#include <island/gui/carry.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "../../score.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

Vector<VIEWS::ROLL::Loose> held;
Vector<VIEWS::ROLL::Loose> afloat;

constexpr STRING::Hot ADRIFT =
  "the pasted notes found no notes lane on their track";
constexpr STRING::Hot PASTED = "paste";

auto pipped(Whole slot) -> VIEWS::ROLL::Pip {
  const VIEWS::ROLL::Loose &one = ::afloat[slot];
  return {NONE, slot, one.pitch, one.at, one.length, one.track};
}

auto facts(const VIEWS::ROLL::Pip &pip, VIEWS::ROLL::Loose &out) -> Flag {
  if (pip.index == NONE) return false;
  if (pip.stock == NONE) {
    if (pip.index >= ::afloat.size()) return false;
    out = ::afloat[pip.index];
    return true;
  }
  const Inventory &pool = INVENTORY::held();
  if (pip.stock >= pool.stocks.size()) return false;
  const Score &score = pool.stocks[pip.stock].score;
  if (pip.index >= score.notes.size()) return false;
  out = {
    pip.pitch, pip.at, pip.length, score.notes[pip.index].velocity, pip.track};
  return true;
}

auto taken(GUI::Handle page) -> Vector<VIEWS::ROLL::Loose> {
  const Vector<VIEWS::ROLL::Pip> &drawn = VIEWS::ROLL::plates();
  Vector<VIEWS::ROLL::Loose> facts;
  for (const Whole row : VIEWS::ROLL::chosen(page)) {
    VIEWS::ROLL::Loose one;
    if (row < drawn.size() && ::facts(drawn[row], one)) facts.push_back(one);
  }
  return facts;
}

struct Fall {
  Whole stock = NONE;
  Score score;
  Vector<SCORE::Landing> landings;
};

auto grouped(Vector<::Fall> &falls, Whole stock) -> ::Fall & {
  for (::Fall &fall : falls)
    if (fall.stock == stock) return fall;
  falls.push_back({stock, INVENTORY::held().stocks[stock].score, {}});
  return falls.back();
}

auto written(Vector<::Fall> &falls, const VIEWS::ROLL::Fallen &fall) -> Flag {
  const VIEWS::ROLL::Loose &one = ::afloat[fall.slot];
  const VIEWS::ROLL::Aim aim = one.track == NONE
                                 ? VIEWS::ROLL::claimed(fall.at)
                                 : VIEWS::ROLL::claimed(one.track, fall.at);
  if (aim.stock == NONE) return false;
  ::Fall &filed = ::grouped(falls, aim.stock);
  const Whole at = VIEWS::ROLL::inside(aim, fall.at);
  const Whole pitch = VIEWS::ROLL::degreed(fall.pitch, fall.at);
  const Whole index = SCORE::write(
    filed.score,
    {.pitch = pitch, .at = at, .length = one.length, .velocity = one.velocity});
  for (SCORE::Landing &prior : filed.landings)
    if (prior.index >= index) ++prior.index;
  filed.landings.push_back({index, at, pitch});
  return true;
}

}  // namespace

auto SOUND::VIEWS::ROLL::chosen(GUI::Handle page) -> Vector<Whole> {
  const String board = VIEWS::ROLL::board();
  Vector<Whole> rows;
  for (const STRING::Cold &id :
       GUI::NGA::GET::selections(page, board.c_str())) {
    const Whole row = GUI::SAC::SEAT::row(String(id), board.c_str());
    if (row == NONE || GUI::SAC::SEAT::named(board.c_str(), row) != id)
      continue;
    rows.push_back(row);
  }
  std::sort(rows.begin(), rows.end());
  rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
  return rows;
}

void SOUND::VIEWS::ROLL::copy(GUI::Handle page) {
  const Vector<Loose> facts = ::taken(page);
  if (!facts.empty()) ::held = facts;
  ::afloat.clear();
}

void SOUND::VIEWS::ROLL::paste(GUI::Handle) {
  if (::held.empty()) return;
  ::afloat = ::held;
  Vector<Pip> marks;
  for (Whole slot = 0; slot < ::afloat.size(); ++slot)
    marks.push_back(::pipped(slot));
  follow(marks);
}

void SOUND::VIEWS::ROLL::grasped(GUI::Handle page) {
  if (::afloat.empty()) return;
  const String board = VIEWS::ROLL::board();
  const GUI::NGA::Ask press = GUI::NGA::GET::pressed(page, board.c_str());
  if (press.target.empty()) return;
  const Whole row = GUI::SAC::SEAT::row(String(press.target), board.c_str());
  const Vector<Pip> &drawn = plates();
  if (row >= drawn.size() || drawn[row].stock != NONE) return;
  if (drawn[row].index == NONE) return;
  Vector<String> marks;
  for (Whole one = 0; one < drawn.size(); ++one)
    if (drawn[one].stock == NONE && drawn[one].index != NONE)
      marks.push_back(GUI::SAC::SEAT::named(board.c_str(), one));
  GUI::SAC::CARRY::follow(page, marks);
}

auto SOUND::VIEWS::ROLL::loose() -> const Vector<Loose> & { return ::afloat; }

void SOUND::VIEWS::ROLL::loosed(Vector<Pip> &wanted) {
  for (Whole slot = 0; slot < ::afloat.size(); ++slot)
    wanted.push_back(::pipped(slot));
}

auto SOUND::VIEWS::ROLL::land(GUI::Handle page, const Vector<Fallen> &fallen)
  -> Vector<Pip> {
  Vector<::Fall> falls;
  Vector<Whole> gone;
  Whole refused = 0;
  for (const Fallen &fall : fallen) {
    if (fall.slot >= ::afloat.size()) continue;
    if (::written(falls, fall))
      gone.push_back(fall.slot);
    else
      ++refused;
  }
  if (!fallen.empty())
    GUI::set(page, NOTICE, GUI::Text{refused == 0 ? "" : ::ADRIFT});
  Vector<Pip> landed;
  HISTORY::begin();
  for (::Fall &fall : falls) {
    const Vector<Whole> stood = SCORE::land(fall.score, fall.landings);
    HISTORY::take(fall.stock, INVENTORY::held().stocks[fall.stock]);
    INVENTORY::score(fall.stock, fall.score);
    HISTORY::wrote(::PASTED, fall.stock, INVENTORY::held().stocks[fall.stock]);
    for (const Whole index : stood)
      if (index != NONE) landed.push_back({fall.stock, index});
  }
  HISTORY::end();
  discard(gone);
  return landed;
}

void SOUND::VIEWS::ROLL::stretched(Whole slot, Whole length) {
  if (slot < ::afloat.size()) ::afloat[slot].length = length;
}

void SOUND::VIEWS::ROLL::discard(const Vector<Whole> &slots) {
  Vector<Whole> order = slots;
  std::sort(order.begin(), order.end(), std::greater<Whole>());
  for (const Whole slot : order)
    if (slot < ::afloat.size())
      ::afloat.erase(::afloat.begin() + static_cast<Integer>(slot));
}

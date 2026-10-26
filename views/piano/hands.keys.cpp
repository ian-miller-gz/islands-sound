// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/carry.hpp>
#include <island/gui/seat.hpp>
#include <island/input.hpp>

#include "../../inventory.hpp"
#include "piano.internal.hpp"

#include <algorithm>

namespace {
using namespace SOUND;

constexpr Whole COPY = 'C';
constexpr Whole PASTE = 'V';
constexpr Whole CASED = 'a' - 'A';
constexpr STRING::Hot ERASED = "erase";

void taken(Whole stock) {
  HISTORY::take(stock, INVENTORY::held().stocks[stock]);
}

void wrote(Whole stock) {
  HISTORY::wrote(::ERASED, stock, INVENTORY::held().stocks[stock]);
}

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

}  // namespace

auto SOUND::VIEWS::ROLL::pluck(GUI::Handle page, Whole row) -> Flag {
  if (!GUI::SAC::CARRY::GET::carried(page).empty()) return false;
  const Vector<Pip> &drawn = plates();
  if (row >= drawn.size() || drawn[row].index == NONE) return false;
  if (drawn[row].stock == NONE) {
    discard({drawn[row].index});
    return true;
  }
  if (drawn[row].stock >= INVENTORY::held().stocks.size()) return false;
  Score score = INVENTORY::held().stocks[drawn[row].stock].score;
  if (!SCORE::erase(score, drawn[row].index)) return false;
  ::taken(drawn[row].stock);
  INVENTORY::score(drawn[row].stock, score);
  ::wrote(drawn[row].stock);
  return true;
}

void SOUND::VIEWS::ROLL::plucked(GUI::Handle page) {
  if (!GUI::SAC::CARRY::GET::carried(page).empty()) return;
  const String board = VIEWS::ROLL::board();
  const Vector<Pip> &drawn = plates();
  Vector<Pip> chosen;
  Vector<Whole> slots;
  for (const Whole row : VIEWS::ROLL::chosen(page)) {
    if (row >= drawn.size() || drawn[row].index == NONE) continue;
    if (drawn[row].stock == NONE)
      slots.push_back(drawn[row].index);
    else
      chosen.push_back(drawn[row]);
  }
  discard(slots);
  std::sort(chosen.begin(), chosen.end(), [](const Pip &a, const Pip &b) {
    return a.stock != b.stock ? a.stock < b.stock : a.index > b.index;
  });
  HISTORY::begin();
  for (const Pip &pip : chosen) {
    if (pip.stock >= INVENTORY::held().stocks.size()) continue;
    Score score = INVENTORY::held().stocks[pip.stock].score;
    if (!SCORE::erase(score, pip.index)) continue;
    ::taken(pip.stock);
    INVENTORY::score(pip.stock, score);
    ::wrote(pip.stock);
  }
  HISTORY::end();
}

void SOUND::VIEWS::ROLL::keyed() {
  const GUI::Handle page = VIEWS::document();
  const INPUT::KEYS::Event key = GUI::GET::keyed(page);
  if (VIEWS::journaled(key)) return;
  if (key.action == INPUT::KEYS::DELETE) return plucked(page);
  if (key.action != INPUT::KEYS::TEXT || !key.control) return;
  const Whole letter = ::folded(key.codepoint);
  if (letter == ::COPY) return copy(page);
  if (letter == ::PASTE) paste(page);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto seam() -> Float { return VIEWS::SIGNALS::deep() * VIEWS::SIGNALS::HALF; }

struct Hold {
  Whole row = NONE;
  VIEWS::SIGNALS::Point point;
};
Hold holding;

auto plate(Whole row) -> String {
  return GUI::SAC::SEAT::named(VIEWS::SIGNALS::board().c_str(), row);
}

void grabbed(GUI::Handle page) {
  const STRING::Cold box = GUI::SAC::CARRY::GET::carried(page);
  if (box.empty()) return (void)(::holding = {});
  if (::holding.row != NONE) return;
  const String board = VIEWS::SIGNALS::board();
  const Whole row = GUI::SAC::SEAT::row(String(box), board.c_str());
  const Vector<VIEWS::SIGNALS::Point> &drawn = VIEWS::SIGNALS::shown();
  if (row >= drawn.size() || drawn[row].index == NONE) return;
  if (drawn[row].stock >= INVENTORY::held().stocks.size()) return;
  ::holding = {row, drawn[row]};
}

void dropped(GUI::Handle page) {
  if (GUI::SAC::CARRY::GET::dropped(page).empty()) return;
  const ::Hold hold = ::holding;
  ::holding = {};
  if (hold.row == NONE) return;
  VIEWS::SIGNALS::land(page, hold.point, hold.row);
}

void erased(GUI::Handle page) {
  const String board = VIEWS::SIGNALS::board();
  if (!GUI::GET::activated(page, board.c_str())) return;
  VIEWS::SIGNALS::erase(page, VIEWS::SIGNALS::chosen());
}

void lifted(Vector<VIEWS::SIGNALS::Point> &wanted) {
  if (::holding.row == NONE) return;
  for (auto slot = wanted.begin(); slot != wanted.end(); ++slot)
    if (
      slot->stock == ::holding.point.stock &&
      slot->index == ::holding.point.index && slot->at == ::holding.point.at) {
      wanted.erase(slot);
      break;
    }
  VIEWS::SIGNALS::Point ghost = ::holding.point;
  ghost.index = NONE;
  const Whole row = std::min<Whole>(::holding.row, wanted.size());
  wanted.insert(std::next(wanted.begin(), Integer(row)), ghost);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::watched() -> GUI::SAC::CARRY::Watch {
  const Float seam = ::seam();
  return {
    board().c_str(),
    {GUI::Walls::NONE, GUI::Walls::NONE, -seam, down(0.0f) - seam},
    BLADE,
    SEATED};
}

void SOUND::VIEWS::SIGNALS::hands() {
  const GUI::Handle page = document();
  ::dropped(page);
  ::grabbed(page);
  ::erased(page);
}

void SOUND::VIEWS::SIGNALS::steadied(Vector<Point> &wanted) {
  ::lifted(wanted);
  drafted(wanted);
}

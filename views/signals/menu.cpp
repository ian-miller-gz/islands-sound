// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "../../shape.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ERASES = "Delete";
constexpr STRING::Hot SHAPED = "shape";
constexpr STRING::Hot WHOSE = "signals";

struct Offer {
  Whole shape = NONE;
  STRING::Hot word = "";
};

constexpr Offer OFFERS[] = {
  {SHAPE::HOLD, "Step"},
  {SHAPE::LINE, "Line"},
  {SHAPE::RISE, "Rise"},
  {SHAPE::FALL, "Fall"},
  {SHAPE::EASE, "Ease"}};

Whole about = NONE;

auto mates(Whole row) -> Vector<Whole> {
  const Vector<Whole> rows = VIEWS::SIGNALS::chosen();
  if (std::find(rows.begin(), rows.end(), row) == rows.end()) return {row};
  return rows;
}

auto worded() -> Vector<String> {
  Vector<String> said;
  for (const ::Offer &offer : ::OFFERS) said.push_back(String(offer.word));
  said.push_back(String(::ERASES));
  return said;
}

auto standing(Whole row) -> Whole {
  const VIEWS::SIGNALS::Point &drawn = VIEWS::SIGNALS::shown()[row];
  const Whole shape = INVENTORY::shape(drawn.stock, drawn.index);
  for (Whole at = 0; at < std::size(::OFFERS); ++at)
    if (::OFFERS[at].shape == shape) return at;
  return NONE;
}

void shaped(const Vector<Whole> &rows, Whole shape) {
  const Vector<VIEWS::SIGNALS::Point> &drawn = VIEWS::SIGNALS::shown();
  HISTORY::begin();
  for (const Whole row : rows) {
    const Whole stock = drawn[row].stock;
    HISTORY::take(stock, INVENTORY::held().stocks[stock]);
    INVENTORY::shape(stock, drawn[row].index, shape);
    HISTORY::wrote(::SHAPED, stock, INVENTORY::held().stocks[stock]);
  }
  HISTORY::end();
}

void took(GUI::Handle page, Whole taken, Whole asked) {
  const Vector<Whole> rows = ::mates(asked);
  if (taken >= std::size(::OFFERS))
    return (void)VIEWS::SIGNALS::erase(page, rows);
  ::shaped(rows, ::OFFERS[taken].shape);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::menu() {
  const GUI::Handle page = document();
  if (::about != NONE) {
    const Whole asked = ::about;
    const Whole taken = VIEWS::MENU::taken(::WHOSE);
    if (taken != NONE) {
      ::about = NONE;
      return ::took(page, taken, asked);
    }
    if (!VIEWS::MENU::standing()) ::about = NONE;
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  const Whole row = GUI::SAC::SEAT::row(String(ask.target), board().c_str());
  if (row >= shown().size() || shown()[row].index == NONE) return;
  ::about = row;
  VIEWS::MENU::raise(::worded(), ask.x, ask.y, ::WHOSE, ::standing(row));
}

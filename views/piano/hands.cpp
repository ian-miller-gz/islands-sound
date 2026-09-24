// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include <island/gui/carry.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "grip";
constexpr STRING::Hot MOVED = "move";
constexpr STRING::Hot PULLED = "stretch";
constexpr STRING::Hot MARKED = "selected";
constexpr Float WIDE = 10.0f;
constexpr Float SHARE = 3.0f;
constexpr Float RAISED = 1.0f;
constexpr Float ORIGIN = 0.0f;

Vector<VIEWS::ROLL::Pip> following;

Whole stood = 0;
Whole born = 0;
Flag pulling = false;

struct Hold {
  Whole row = NONE;
  VIEWS::ROLL::Pip pip;
  Integer crossing = 0;
};
Hold holding;

constexpr Float WEST = 0.0f;
constexpr Float NORTH = VIEWS::ROLL::lane(VIEWS::ROLL::HIGHEST);
constexpr Float SOUTH = VIEWS::ROLL::lane(Whole(0));

auto settled(Whole pulses) -> Whole {
  const Whole snap = VIEWS::ROLL::bench().snap;
  return (pulses + snap / 2) / snap * snap;
}

auto plate(Whole row) -> String {
  return GUI::SAC::SEAT::named(VIEWS::ROLL::board().c_str(), row);
}

auto handle(Whole row) -> String { return ::plate(row) + "." + ::STEM; }

auto gripped(Whole length) -> Float {
  return std::min(::WIDE, VIEWS::ROLL::wide(length) / ::SHARE);
}

auto resting(Whole length) -> Float {
  return VIEWS::ROLL::wide(length) - ::gripped(length);
}

auto crossing(const VIEWS::ROLL::Pip &pip) -> Integer {
  const Score &opened = INVENTORY::held().stocks[pip.stock].score;
  if (pip.index >= opened.notes.size()) return 0;
  return Integer(opened.notes[pip.index].at) - Integer(pip.at);
}

auto inside(Whole at, Integer by) -> Whole {
  const Integer wanted = Integer(at) + by;
  return wanted <= 0 ? 0 : Whole(wanted);
}

void grabbed(GUI::Handle page) {
  const STRING::Cold box = GUI::SAC::CARRY::GET::carried(page);
  if (box.empty()) {
    ::holding = {};
    return;
  }
  if (::holding.row != NONE) return;
  const String board = VIEWS::ROLL::board();
  const Whole row = GUI::SAC::SEAT::row(String(box), board.c_str());
  const Vector<VIEWS::ROLL::Pip> &drawn = VIEWS::ROLL::plates();
  if (row >= drawn.size()) return;
  const VIEWS::ROLL::Pip &pip = drawn[row];
  if (pip.index == NONE) return;
  if (pip.stock == NONE) {
    if (pip.index < VIEWS::ROLL::loose().size()) ::holding = {row, pip, 0};
    return;
  }
  if (pip.stock >= INVENTORY::held().stocks.size()) return;
  if (pip.index >= INVENTORY::held().stocks[pip.stock].score.notes.size())
    return;
  ::holding = {row, pip, ::crossing(pip)};
}

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

struct Shift {
  Integer at = 0;
  Integer pitch = 0;
};

auto handed(const Vector<GUI::SAC::CARRY::Drop> &landings, const Hold &hold)
  -> const GUI::SAC::CARRY::Drop * {
  if (hold.row == NONE) return nullptr;
  for (const GUI::SAC::CARRY::Drop &landing : landings)
    if (
      GUI::SAC::SEAT::row(String(landing.id), VIEWS::ROLL::board().c_str()) ==
      hold.row)
      return &landing;
  return nullptr;
}

auto shifted(const Hold &hold, const GUI::SAC::CARRY::Drop &landing)
  -> ::Shift {
  return {
    Integer(::settled(VIEWS::ROLL::across(landing.at.x))) -
      Integer(hold.pip.at),
    Integer(VIEWS::ROLL::lane(landing.at.y)) - Integer(hold.pip.pitch)};
}

auto moved(Whole at, Integer by) -> Whole {
  const Integer wanted = Integer(at) + by;
  return wanted <= 0 ? 0 : Whole(wanted);
}

auto raised(Whole pitch, Integer by, Whole at) -> Whole {
  const Integer wanted = Integer(pitch) + by;
  if (wanted <= 0) return VIEWS::ROLL::degreed(0, at);
  return VIEWS::ROLL::degreed(
    wanted < Integer(VIEWS::ROLL::HIGHEST) ? Whole(wanted)
                                           : VIEWS::ROLL::HIGHEST,
    at);
}

void fell(
  Vector<::Fall> &falls, Vector<VIEWS::ROLL::Fallen> &fallen, const Hold &hold,
  const ::Shift &shift, const GUI::SAC::CARRY::Drop &landing) {
  const Whole row =
    GUI::SAC::SEAT::row(String(landing.id), VIEWS::ROLL::board().c_str());
  const Vector<VIEWS::ROLL::Pip> &drawn = VIEWS::ROLL::plates();
  const Flag leader = row == hold.row;
  if (!leader && row >= drawn.size()) return;
  const VIEWS::ROLL::Pip pip = leader ? hold.pip : drawn[row];
  if (pip.index == NONE) return;
  const Whole landed = ::moved(pip.at, shift.at);
  if (pip.stock == NONE)
    return fallen.push_back(
      {pip.index, landed, ::raised(pip.pitch, shift.pitch, landed)});
  if (pip.stock >= INVENTORY::held().stocks.size()) return;
  if (pip.index >= INVENTORY::held().stocks[pip.stock].score.notes.size())
    return;
  ::grouped(falls, pip.stock)
    .push_back(
      {pip.index, ::inside(landed, leader ? hold.crossing : ::crossing(pip)),
       ::raised(pip.pitch, shift.pitch, landed)});
}

void dropped(GUI::Handle page) {
  const Vector<GUI::SAC::CARRY::Drop> landings =
    GUI::SAC::CARRY::GET::dropped(page);
  if (landings.empty()) return;
  const Hold hold = ::holding;
  ::holding = {};
  const GUI::SAC::CARRY::Drop *hand = ::handed(landings, hold);
  if (hand == nullptr) return;
  const ::Shift shift = ::shifted(hold, *hand);
  Vector<::Fall> falls;
  Vector<VIEWS::ROLL::Fallen> fallen;
  for (const GUI::SAC::CARRY::Drop &landing : landings)
    ::fell(falls, fallen, hold, shift, landing);
  HISTORY::begin();
  for (const ::Fall &fall : falls) {
    Score score = INVENTORY::held().stocks[fall.stock].score;
    const Vector<Whole> landed = SCORE::land(score, fall.landings);
    if (landed.empty()) continue;
    HISTORY::take(fall.stock, INVENTORY::held().stocks[fall.stock]);
    INVENTORY::score(fall.stock, score);
    HISTORY::wrote(::MOVED, fall.stock, INVENTORY::held().stocks[fall.stock]);
    for (Whole one = 0; one < landed.size(); ++one)
      if (landed[one] != NONE) ::following.push_back({fall.stock, landed[one]});
  }
  VIEWS::ROLL::follow(VIEWS::ROLL::land(page, fallen));
  HISTORY::end();
}

struct Reach {
  Whole index = NONE, length = 0;
};

struct Stretch {
  Whole stock = NONE;
  Vector<::Reach> reaches;
};

auto reaching(Vector<::Stretch> &stocks, Whole stock) -> Vector<::Reach> & {
  for (::Stretch &held : stocks)
    if (held.stock == stock) return held.reaches;
  stocks.push_back({stock, {}});
  return stocks.back().reaches;
}

auto grew(Whole length, Integer by) -> Whole {
  const Integer wanted = Integer(length) + by;
  const Integer floor = Integer(VIEWS::ROLL::bench().snap);
  return wanted > floor ? Whole(wanted) : Whole(floor);
}

void sharing(
  GUI::Handle page, Vector<::Stretch> &stocks, Whole pulled, Integer growth) {
  const Vector<VIEWS::ROLL::Pip> &drawn = VIEWS::ROLL::plates();
  for (const Whole row : VIEWS::ROLL::chosen(page)) {
    if (row == pulled || row >= drawn.size()) continue;
    const VIEWS::ROLL::Pip &pip = drawn[row];
    if (pip.index == NONE || pip.stock >= INVENTORY::held().stocks.size())
      continue;
    ::reaching(stocks, pip.stock)
      .push_back({pip.index, ::grew(pip.length, growth)});
  }
}

auto lengthened(const VIEWS::ROLL::Pip &pip, Float x) -> Whole {
  const Float grown =
    Float(pip.length) / Float(VIEWS::ROLL::GRAIN) + x - ::resting(pip.length);
  return std::max(
    VIEWS::ROLL::bench().snap, ::settled(VIEWS::ROLL::across(grown)));
}

void pulled(GUI::Handle page, Whole row, const VIEWS::ROLL::Pip &pip, Float x) {
  const Whole length = ::lengthened(pip, x);
  Vector<::Stretch> stocks;
  ::reaching(stocks, pip.stock).push_back({pip.index, length});
  if (GUI::NGA::GET::selected(page, ::plate(row).c_str()))
    ::sharing(page, stocks, row, Integer(length) - Integer(pip.length));
  for (const ::Stretch &stock : stocks) {
    Score score = INVENTORY::held().stocks[stock.stock].score;
    Flag moved = false;
    for (const ::Reach &reach : stock.reaches)
      if (SCORE::stretch(score, reach.index, reach.length)) moved = true;
    if (!moved) continue;
    HISTORY::take(stock.stock, INVENTORY::held().stocks[stock.stock]);
    INVENTORY::score(stock.stock, score);
    HISTORY::wrote(
      ::PULLED, stock.stock, INVENTORY::held().stocks[stock.stock]);
  }
}

void erased(GUI::Handle page) {
  const String board = VIEWS::ROLL::board();
  if (!GUI::GET::activated(page, board.c_str())) return;
  VIEWS::ROLL::pluck(page, GUI::SAC::SEAT::selected(page, board.c_str()));
}

void lifted(Vector<VIEWS::ROLL::Pip> &wanted) {
  if (::holding.row == NONE) return;
  for (auto slot = wanted.begin(); slot != wanted.end(); ++slot)
    if (
      slot->stock == ::holding.pip.stock &&
      slot->index == ::holding.pip.index && slot->at == ::holding.pip.at) {
      wanted.erase(slot);
      break;
    }
  VIEWS::ROLL::Pip ghost = ::holding.pip;
  ghost.index = NONE;
  const Whole row = std::min<Whole>(::holding.row, wanted.size());
  wanted.insert(std::next(wanted.begin(), Integer(row)), ghost);
}

void followed(GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted) {
  if (::following.empty()) return;
  Vector<String> marks;
  for (Whole row = 0; row < wanted.size(); ++row)
    for (const VIEWS::ROLL::Pip &pip : ::following)
      if (wanted[row].stock == pip.stock && wanted[row].index == pip.index) {
        marks.push_back(::plate(row));
        break;
      }
  GUI::SAC::CARRY::follow(page, marks);
  ::following.clear();
}

void handles(GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &drawn) {
  if (VIEWS::reborn(::born)) ::stood = 0;
  ::stood = std::min<Whole>(::stood, drawn.size());
  for (; ::stood < drawn.size(); ++::stood)
    BOARDS::place(
      page, ::plate(::stood).c_str(), "node", ::handle(::stood), {}, {});
  for (Whole row = 0; row < drawn.size(); ++row) {
    const String id = ::handle(row);
    GUI::set(
      page, id.c_str(), GUI::Position{::resting(drawn[row].length), ::ORIGIN});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{::gripped(drawn[row].length), VIEWS::ROLL::TALL});
    GUI::set(page, id.c_str(), GUI::Style{::MARKED});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
}

}  // namespace

auto SOUND::VIEWS::ROLL::watched() -> GUI::SAC::CARRY::Watch {
  return {
    board().c_str(),
    {::WEST, GUI::Walls::NONE, ::NORTH, ::SOUTH},
    HELD,
    SEATED};
}

void SOUND::VIEWS::ROLL::SHED::hands() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  for (Whole row = 0; row < ::stood; ++row) BOARDS::drop(page, ::handle(row));
  ::stood = 0;
}

void SOUND::VIEWS::ROLL::hands() {
  const GUI::Handle page = document();
  const Vector<Pip> &drawn = plates();
  Flag moving = false;
  for (Whole row = 0; row < drawn.size(); ++row) {
    if (drawn[row].index == NONE) continue;
    if (!GUI::GET::moved(page, ::handle(row).c_str())) continue;
    if (!::pulling) {
      HISTORY::begin();
      ::pulling = true;
    }
    moving = true;
    const Float x = GUI::GET::position(page, ::handle(row).c_str()).x;
    if (drawn[row].stock == NONE)
      stretched(drawn[row].index, ::lengthened(drawn[row], x));
    else if (drawn[row].stock < INVENTORY::held().stocks.size())
      ::pulled(page, row, drawn[row], x);
  }
  if (!moving && ::pulling) {
    HISTORY::end();
    ::pulling = false;
  }
  grasped(page);
  ::dropped(page);
  ::grabbed(page);
  ::erased(page);
  ::handles(page, drawn);
}

void SOUND::VIEWS::ROLL::follow(const Vector<Pip> &landed) {
  ::following.insert(::following.end(), landed.begin(), landed.end());
}

void SOUND::VIEWS::ROLL::steadied(GUI::Handle page, Vector<Pip> &wanted) {
  ::lifted(wanted);
  ::followed(page, wanted);
}

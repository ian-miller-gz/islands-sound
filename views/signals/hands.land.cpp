// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

struct Shift {
  Integer at = 0;
  Float value = 0.0f;
};

struct Fall {
  Whole stock = NONE, index = NONE, at = 0;
  Integer crossing = 0;
  Float value = 0.0f;
  Whole shape = SHAPE::HOLD;
};

Vector<VIEWS::SIGNALS::Point> following;

constexpr STRING::Hot MOVED = "move";

auto settled(Whole frames) -> Whole {
  const Whole snap = VIEWS::SIGNALS::grain();
  return (frames + snap / 2) / snap * snap;
}

auto framed(Float x) -> Whole {
  const Float at = (x + VIEWS::SIGNALS::wide() * VIEWS::SIGNALS::HALF) *
                   Float(VIEWS::SIGNALS::GRAIN);
  return at <= 0.0f ? 0 : Whole(at);
}

auto valued(Float y) -> Float {
  const Float down =
    (y + VIEWS::SIGNALS::deep() * VIEWS::SIGNALS::HALF) / VIEWS::SIGNALS::STEP;
  return std::clamp(
    Float(VIEWS::SIGNALS::FULL) - std::round(down), 0.0f,
    Float(VIEWS::SIGNALS::FULL));
}

auto crossed(const VIEWS::SIGNALS::Point &point) -> Integer {
  const VIEWS::SIGNALS::Row held =
    VIEWS::SIGNALS::row(point.stock, point.index);
  if (held.at == NONE) return 0;
  return Integer(held.at) - Integer(point.at);
}

auto moved(Whole at, Integer by) -> Whole {
  const Integer wanted = Integer(at) + by;
  return wanted <= 0 ? 0 : Whole(wanted);
}

auto handed(const Vector<GUI::SAC::CARRY::Drop> &drops, Whole row)
  -> const GUI::SAC::CARRY::Drop * {
  const String board = VIEWS::SIGNALS::board();
  for (const GUI::SAC::CARRY::Drop &drop : drops)
    if (GUI::SAC::SEAT::row(String(drop.id), board.c_str()) == row)
      return &drop;
  return nullptr;
}

void fell(
  Vector<::Fall> &falls, const VIEWS::SIGNALS::Point &held, Whole leader,
  const ::Shift &shift, const GUI::SAC::CARRY::Drop &drop) {
  const String board = VIEWS::SIGNALS::board();
  const Whole row = GUI::SAC::SEAT::row(String(drop.id), board.c_str());
  const Vector<VIEWS::SIGNALS::Point> &drawn = VIEWS::SIGNALS::shown();
  if (row != leader && row >= drawn.size()) return;
  const VIEWS::SIGNALS::Point point = row == leader ? held : drawn[row];
  if (point.index == NONE) return;
  const VIEWS::SIGNALS::Row standing =
    VIEWS::SIGNALS::row(point.stock, point.index);
  if (standing.at == NONE) return;
  falls.push_back(
    {point.stock, point.index, ::moved(point.at, shift.at), ::crossed(point),
     std::clamp(point.value + shift.value, 0.0f, Float(VIEWS::SIGNALS::FULL)),
     standing.shape});
}

void wrote(Vector<::Fall> &falls) {
  std::sort(
    falls.begin(), falls.end(), [](const ::Fall &one, const ::Fall &other) {
      return one.index > other.index;
    });
  HISTORY::begin();
  for (const ::Fall &fall : falls) {
    HISTORY::take(fall.stock, INVENTORY::held().stocks[fall.stock]);
    INVENTORY::erase(fall.stock, fall.index);
  }
  for (const ::Fall &fall : falls) {
    VIEWS::SIGNALS::write(
      fall.stock, {::moved(fall.at, fall.crossing), fall.value, fall.shape});
    HISTORY::wrote(::MOVED, fall.stock, INVENTORY::held().stocks[fall.stock]);
    ::following.push_back(
      {fall.stock, NONE, fall.at, 0, fall.value, fall.shape});
  }
  HISTORY::end();
}

}  // namespace

void SOUND::VIEWS::SIGNALS::land(
  GUI::Handle page, const Point &held, Whole row) {
  const Vector<GUI::SAC::CARRY::Drop> drops =
    GUI::SAC::CARRY::GET::dropped(page);
  const GUI::SAC::CARRY::Drop *hand = ::handed(drops, row);
  if (hand == nullptr) return;
  const ::Shift shift = {
    Integer(::settled(::framed(hand->at.x))) - Integer(held.at),
    ::valued(hand->at.y) - held.value};
  Vector<::Fall> falls;
  for (const GUI::SAC::CARRY::Drop &drop : drops)
    ::fell(falls, held, row, shift, drop);
  ::wrote(falls);
}

void SOUND::VIEWS::SIGNALS::followed(
  GUI::Handle page, const Vector<Point> &wanted) {
  if (::following.empty()) return;
  Vector<String> marks;
  for (Whole row = 0; row < wanted.size(); ++row)
    for (const Point &point : ::following)
      if (wanted[row].stock == point.stock && wanted[row].at == point.at) {
        marks.push_back(GUI::SAC::SEAT::named(board().c_str(), row));
        break;
      }
  GUI::SAC::CARRY::follow(page, marks);
  ::following.clear();
}

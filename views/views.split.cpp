// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::Seat;
using VIEWS::SPLIT::SHUT;

constexpr STRING::Hot HEIGHT = "split.height";
constexpr STRING::Hot VIEW = "split.view";
Float height = SHUT, written = SHUT;
Flag known = false;
Whole born = 0;

void handled(GUI::Handle page, const Seat &seat) {
  const Float y = GUI::GET::position(page, VIEWS::SPLIT::BAR).y;
  if (::known && y != ::written)
    ::height = std::clamp(seat.foot - y, ::SHUT, seat.room);
  if (GUI::GET::moved(page, VIEWS::SPLIT::BAR) && !VIEWS::SPLIT::open())
    ::height = ::SHUT;
}

void seated(GUI::Handle page, const Seat &seat, Float tall) {
  const GUI::Position at = GUI::GET::position(page, VIEWS::SPLIT::BAR);
  ::written = seat.foot - tall;
  ::known = true;
  if (at.y != ::written)
    GUI::set(page, VIEWS::SPLIT::BAR, GUI::Position{at.x, ::written});
  const Flag on = VIEWS::SPLIT::open();
  if (GUI::GET::visibility(page, VIEWS::SPLIT::BAND) != on)
    GUI::set(page, VIEWS::SPLIT::BAND, GUI::Visibility{on});
}

}  // namespace

auto SOUND::VIEWS::SPLIT::height() -> Float { return ::height; }

void SOUND::VIEWS::SPLIT::frame() {
  const GUI::Handle page = sheet();
  if (reborn(::born)) ::known = false;
  const Seat seat = measured(page);
  if (seat.depth <= ::SHUT || seat.bar <= ::SHUT) return;
  ::handled(page, seat);
  const Float tall = std::min(::height, seat.room);
  ::seated(page, seat, tall);
  chose();
  rooted(page, seat, tall);
}

void SOUND::VIEWS::SPLIT::keep(FIELDS::Map &face) {
  face[::HEIGHT] = ::height;
  const Whole at = lower();
  if (at != NONE) face[::VIEW] = String(roster()[at].name);
}

void SOUND::VIEWS::SPLIT::restore(const FIELDS::Map &face) {
  KEPT::number(face, ::HEIGHT, ::height);
  String name;
  if (KEPT::text(face, ::VIEW, name)) choose(name);
}

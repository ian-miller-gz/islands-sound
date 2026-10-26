// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::Seat;
using VIEWS::SPLIT::SHUT;
using VIEWS::SPLIT::UPPER;

Float height = SHUT, written = SHUT, last = SHUT;
Flag known = false, wide = false;
Whole born = 0;

void edged() {
  const Flag now = VIEWS::SPLIT::open();
  if (now != ::wide) VIEWS::SPLIT::focus(now ? BELOW : UPPER);
  ::wide = now;
}

void handled(GUI::Handle page, const Seat &seat) {
  const Float y = GUI::GET::position(page, VIEWS::SPLIT::BAR).y;
  if (::known && y != ::written)
    ::height = std::clamp(seat.foot - y, ::SHUT, seat.room);
  if (!GUI::GET::moved(page, VIEWS::SPLIT::BAR)) return;
  if (VIEWS::SPLIT::open())
    ::last = ::height;
  else
    ::height = ::SHUT;
  ::edged();
}

void toggled(GUI::Handle page) {
  const Flag bar = GUI::GET::activated(page, VIEWS::SPLIT::BAR);
  const Flag strip = GUI::GET::clicked(page, VIEWS::SPLIT::TOGGLE);
  if (bar || strip) VIEWS::SPLIT::toggle();
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
  keyed();
  const GUI::Handle page = sheet();
  if (reborn(::born)) ::known = false;
  const Seat seat = measured(page);
  if (seat.depth <= ::SHUT || seat.bar <= ::SHUT) return;
  pressed(page);
  ::handled(page, seat);
  ::toggled(page);
  const Float tall = std::min(::height, seat.room);
  ::seated(page, seat, tall);
  grounded(page, seat, tall);
  chose();
  rooted(page, seat, tall);
}

auto SOUND::VIEWS::SPLIT::last() -> Float { return ::last; }

void SOUND::VIEWS::SPLIT::toggle() {
  const Seat seat = measured(sheet());
  const Float back = ::last > ::SHUT ? ::last : seat.depth * SHARE;
  ::height = open() ? ::SHUT : std::min(back, seat.room);
  ::edged();
}

void SOUND::VIEWS::SPLIT::seat(Float tall) {
  ::height = std::clamp(tall, ::SHUT, measured(sheet()).room);
  if (open()) ::last = ::height;
  ::edged();
}

void SOUND::VIEWS::SPLIT::restore(Float tall, Float kept) {
  ::height = tall;
  ::last = kept;
  ::wide = open();
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "../../transport.hpp"
#include "signals.internal.hpp"

auto SOUND::VIEWS::SIGNALS::window() -> Float {
  const GUI::Handle page = document();
  const String id = board();
  return GUI::SAC::WINDOW::seen(
           GUI::GET::measured(page, id.c_str()),
           GUI::NGA::GET::zoom(page, id.c_str()))
    .w;
}

auto SOUND::VIEWS::SIGNALS::least() -> GUI::Extent {
  const GUI::Extent seen = GUI::GET::measured(document(), board().c_str());
  return {
    GUI::SAC::WINDOW::least(seen.w, across(REACH)),
    GUI::SAC::WINDOW::least(seen.h, DEEP)};
}

void SOUND::VIEWS::SIGNALS::bound() {
  GUI::NGA::set(
    document(), board().c_str(),
    GUI::NGA::Bounds{ORIGIN, GUI::NGA::Bounds::NONE, ORIGIN, DEEP});
}

void SOUND::VIEWS::SIGNALS::chase() {
  const GUI::Handle page = document();
  const String id = board();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, id.c_str());
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Pan{
      VIEWS::chased(held.x, across(VIEWS::clock()), window()), held.y});
}

auto SOUND::VIEWS::SIGNALS::seen() -> Span {
  const Float wide = window();
  if (wide <= ORIGIN) return {};
  const Float west = GUI::NGA::GET::pan(document(), board().c_str()).x;
  const Float from = west > ORIGIN ? west : ORIGIN;
  return {
    NONE, Whole(from * Float(GRAIN)), Whole((from + wide) * Float(GRAIN))};
}

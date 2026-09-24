// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "lane.internal.hpp"

auto SOUND::VIEWS::LANE::field() -> String {
  return String(faced()->name) + "." + BOARD;
}

auto SOUND::VIEWS::LANE::windowed() -> Float {
  const GUI::Handle page = document();
  const String id = field();
  return GUI::SAC::WINDOW::seen(
           GUI::GET::measured(page, id.c_str()),
           GUI::NGA::GET::zoom(page, id.c_str()))
    .w;
}

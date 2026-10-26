// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "lane.internal.hpp"

namespace {

constexpr STRING::Hot MAPPED = "minimap";
constexpr STRING::Hot SLIDING = "slide";

}  // namespace

auto SOUND::VIEWS::LANE::field() -> String {
  return String(faced()->name) + "." + BOARD;
}

auto SOUND::VIEWS::LANE::field(Whole kind) -> String {
  return String(faced(kind)->name) + "." + BOARD;
}

auto SOUND::VIEWS::LANE::windowed() -> Float {
  const GUI::Handle page = document();
  const String id = field();
  return GUI::SAC::WINDOW::seen(
           GUI::GET::measured(page, id.c_str()),
           GUI::NGA::GET::zoom(page, id.c_str()))
    .w;
}

auto SOUND::VIEWS::LANE::map() -> String {
  return String(face().name) + "." + ::MAPPED;
}

auto SOUND::VIEWS::LANE::slide() -> String {
  return String(face().name) + "." + ::SLIDING;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.paper.hpp"
#include "piano.staff.hpp"

namespace {

constexpr Float ORIGIN = 0.0f;
constexpr STRING::Hot RULER = "ruler";

Float kept = SOUND::VIEWS::ROLL::RAILED;

}  // namespace

auto SOUND::VIEWS::ROLL::window() -> GUI::Extent {
  const GUI::Handle page = document();
  const String id = board();
  return GUI::SAC::WINDOW::seen(
    GUI::GET::measured(page, id.c_str()),
    GUI::NGA::GET::zoom(page, id.c_str()));
}

auto SOUND::VIEWS::ROLL::least() -> GUI::Extent {
  const GUI::Extent seen = GUI::GET::measured(document(), board().c_str());
  const Float wide = GUI::SAC::WINDOW::least(seen.w, across(REACH));
  if (GET::notation() == PERCUSSION) return {wide, STAFF::SCALE};
  return {wide, GUI::SAC::WINDOW::least(seen.h, ROWS::deep())};
}

auto SOUND::VIEWS::ROLL::most() -> GUI::Extent {
  if (GET::notation() == PERCUSSION) return {.h = STAFF::SCALE};
  return {};
}

auto SOUND::VIEWS::ROLL::scaled() -> Float {
  return GUI::NGA::GET::zoom(document(), board().c_str()).value;
}

void SOUND::VIEWS::ROLL::bound() {
  const GUI::Handle page = document();
  const String id = board();
  const Float walled = GUI::NGA::GET::bounds(page, id.c_str()).west;
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id.c_str());
  const Float west = -margin() / scaled();
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Bounds{west, GUI::NGA::Bounds::NONE, ::ORIGIN, ROWS::deep()});
  if (margin() > ::kept && pan.x == walled)
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Pan{west, pan.y});
  ::kept = margin();
}

auto SOUND::VIEWS::ROLL::strip() -> String {
  return String(face().name) + "." + ::RULER;
}

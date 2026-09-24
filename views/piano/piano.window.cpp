// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "piano.internal.hpp"

namespace {

constexpr Float ORIGIN = 0.0f;

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
  return {
    GUI::SAC::WINDOW::least(seen.w, across(REACH)),
    GUI::SAC::WINDOW::least(seen.h, DEEP)};
}

void SOUND::VIEWS::ROLL::bound() {
  GUI::NGA::set(
    document(), board().c_str(),
    GUI::NGA::Bounds{
      -RAILED / scaled(), GUI::NGA::Bounds::NONE, ::ORIGIN, DEEP});
}

void SOUND::VIEWS::ROLL::park() {
  GUI::NGA::set(
    document(), board().c_str(), GUI::NGA::Pan{-RAILED / scaled(), ::ORIGIN});
}

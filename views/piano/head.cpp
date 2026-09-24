// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>
#include <island/gui/window.hpp>

#include "../../boards.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "head";
constexpr STRING::Hot MARKED = "selected";
constexpr Float HAIR = 2.0f;
constexpr Float NORTH = 0.0f;
constexpr Float RAISED = 1.0f;

Flag stood = false;
Whole born = 0;

auto pulse() -> Whole {
  return SCORE::pulsed(VIEWS::clock(), TRANSPORT::held().tempos);
}

void seat(GUI::Handle page, const String &id) {
  BOARDS::place(
    page, VIEWS::ROLL::board().c_str(), "panel", id, {},
    {::HAIR, VIEWS::ROLL::DEEP});
  GUI::set(page, id.c_str(), GUI::Style{::MARKED});
  GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  ::stood = true;
}

}  // namespace

void SOUND::VIEWS::ROLL::chase() {
  const GUI::Handle page = document();
  const String id = board();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, id.c_str());
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Pan{
      VIEWS::chased(held.x, across(::pulse()), window().w), held.y});
}

void SOUND::VIEWS::ROLL::SHED::head() {
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) return;
  BOARDS::drop(document(), board() + "." + ::STEM);
  ::stood = false;
}

void SOUND::VIEWS::ROLL::head() {
  const GUI::Handle page = document();
  const String id = board() + "." + ::STEM;
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) ::seat(page, id);
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const GUI::SAC::PLAYHEAD::Mark mark =
    GUI::SAC::PLAYHEAD::blade(across(::pulse()), scale, ::HAIR);
  GUI::set(page, id.c_str(), GUI::Position{mark.at, ::NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{mark.wide, VIEWS::ROLL::DEEP});
}

void SOUND::VIEWS::ROLL::head(SHELL::Session &session) {
  session.print(std::format(
    "head pulse {} frame {}", ::pulse(), TRANSPORT::marker().position));
}

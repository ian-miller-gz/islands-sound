// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>

#include "../../boards.hpp"
#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "head";
constexpr STRING::Hot PANEL = "panel";
constexpr STRING::Hot MARKED = "selected";

Flag stood = false;
Whole born = 0;

void seat(GUI::Handle page, const String &id) {
  BOARDS::place(
    page, VIEWS::SIGNALS::board().c_str(), ::PANEL, id, {},
    {VIEWS::SIGNALS::HAIR, VIEWS::SIGNALS::DEEP});
  GUI::set(page, id.c_str(), GUI::Style{::MARKED});
  GUI::set(page, id.c_str(), GUI::Depth{VIEWS::SIGNALS::BLADE});
  ::stood = true;
}

}  // namespace

void SOUND::VIEWS::SIGNALS::SHED::head() {
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) return;
  BOARDS::drop(document(), board() + "." + ::STEM);
  ::stood = false;
}

void SOUND::VIEWS::SIGNALS::head() {
  const GUI::Handle page = document();
  const String id = board() + "." + ::STEM;
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) ::seat(page, id);
  const GUI::SAC::PLAYHEAD::Mark mark =
    GUI::SAC::PLAYHEAD::blade(across(VIEWS::clock()), scaled(), HAIR);
  GUI::set(page, id.c_str(), GUI::Position{mark.at, NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{mark.wide, DEEP});
}

void SOUND::VIEWS::SIGNALS::head(SHELL::Session &session) {
  session.print(std::format(
    "head frame {} at {:g}", TRANSPORT::marker().position,
    across(TRANSPORT::marker().position)));
}

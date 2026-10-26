// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../kind.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MODE = "draw";
constexpr STRING::Hot LAYS = "place";
constexpr STRING::Hot DRAW = "signals.draw";
constexpr STRING::Hot PICK = "signals.pick";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";

Flag drawing = false;
Whole shape = NONE;
Whole wrote = 0;

auto worded() -> STRING::Hot {
  return VIEWS::SIGNALS::paged() == KIND::CONTROL ? ::MODE : ::LAYS;
}

auto standing() -> Whole {
  return VIEWS::SIGNALS::paged() == KIND::CONTROL ? ::shape : NONE;
}

void stated(GUI::Handle page) {
  const String board = VIEWS::SIGNALS::board();
  GUI::SAC::STROKE::draw(
    page,
    ::drawing
      ? GUI::SAC::STROKE::
          Mode{board.c_str(), ::worded(), {VIEWS::SIGNALS::across(VIEWS::SIGNALS::grain()), VIEWS::SIGNALS::STEP}, GUI::SAC::STROKE::Mode::BOARD}
      : GUI::SAC::STROKE::Mode{});
  GUI::SAC::CARRY::watch(
    page, ::drawing ? GUI::SAC::CARRY::Watch{} : VIEWS::SIGNALS::watched());
}

void taken(GUI::Handle page) {
  if (GUI::GET::clicked(page, ::PICK)) ::drawing = false;
  if (GUI::GET::clicked(page, ::DRAW)) {
    ::drawing = true;
    ::shape = NONE;
  }
  const Vector<VIEWS::SIGNALS::Shape> &family = VIEWS::SIGNALS::shapes();
  for (Whole row = 0; row < family.size(); ++row) {
    if (!GUI::GET::clicked(page, family[row].id)) continue;
    ::drawing = true;
    ::shape = row;
  }
}

void worn(GUI::Handle page) {
  const Whole stands = ::standing();
  GUI::set(page, ::PICK, GUI::Style{::drawing ? ::PLAIN : ::MARKED});
  GUI::set(
    page, ::DRAW, GUI::Style{::drawing && stands == NONE ? ::MARKED : ::PLAIN});
  const Vector<VIEWS::SIGNALS::Shape> &family = VIEWS::SIGNALS::shapes();
  const Flag shown = VIEWS::SIGNALS::paged() == KIND::CONTROL;
  for (Whole row = 0; row < family.size(); ++row) {
    GUI::set(
      page, family[row].id,
      GUI::Style{::drawing && stands == row ? ::MARKED : ::PLAIN});
    GUI::set(page, family[row].id, GUI::Visibility{shown});
  }
}

void filled(GUI::Handle page, const GUI::SAC::STROKE::Gesture &gesture) {
  if (VIEWS::SIGNALS::paged() != KIND::CONTROL) {
    if (gesture.opened) VIEWS::SIGNALS::drawn({gesture.from.cell});
    return;
  }
  if (::standing() == NONE)
    return VIEWS::SIGNALS::drawn(GUI::SAC::STROKE::GET::laid(page));
  VIEWS::SIGNALS::spanned(
    gesture.from.cell, gesture.to.cell,
    VIEWS::SIGNALS::shapes()[::standing()].shape);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::easel() {
  const GUI::Handle page = document();
  ::taken(page);
  ::worn(page);
  ::stated(page);
  if (!::drawing) return;
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(page);
  if (!gesture.standing && !gesture.closed) return;
  if (gesture.opened) begun();
  ::filled(page, gesture);
  preview(gesture);
  if (gesture.closed) ::wrote = landed();
}

void SOUND::VIEWS::SIGNALS::easel(SHELL::Session &session) {
  const STRING::Cold mode = GUI::SAC::STROKE::GET::mode(document());
  session.print(std::format(
    "easel mode {} laid {}", mode.empty() ? String("select") : String(mode),
    ::wrote));
  preview(session);
}

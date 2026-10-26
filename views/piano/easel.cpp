// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/carry.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/stroke.hpp>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MODE = "draw";
constexpr STRING::Hot DRAW = "piano.draw";
constexpr STRING::Hot PICK = "piano.pick";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";
constexpr Float STILL = 0.0f;

Flag drawing = false;
Flag stroking = false;

void stroked(Flag standing) {
  if (standing == ::stroking) return;
  ::stroking = standing;
  if (standing)
    HISTORY::begin();
  else
    HISTORY::end();
}

auto railed() -> Float {
  const Float scale = VIEWS::ROLL::scaled();
  return scale > ::STILL ? VIEWS::ROLL::RAILED / scale : VIEWS::ROLL::RAILED;
}

void stated(GUI::Handle page) {
  const String board = VIEWS::ROLL::board();
  GUI::SAC::STROKE::draw(
    page,
    ::drawing
      ? GUI::SAC::STROKE::
          Mode{board.c_str(), ::MODE, {VIEWS::ROLL::across(VIEWS::ROLL::grained()), VIEWS::ROLL::STEP}, GUI::SAC::STROKE::Mode::BOARD}
      : GUI::SAC::STROKE::Mode{});
  GUI::SAC::CARRY::watch(
    page, ::drawing ? GUI::SAC::CARRY::Watch{} : VIEWS::ROLL::watched());
}

void tools(GUI::Handle page) {
  if (GUI::GET::clicked(page, ::DRAW)) ::drawing = true;
  if (GUI::GET::clicked(page, ::PICK)) ::drawing = false;
  GUI::set(page, ::DRAW, GUI::Style{::drawing ? ::MARKED : ::PLAIN});
  GUI::set(page, ::PICK, GUI::Style{::drawing ? ::PLAIN : ::MARKED});
}

auto fielded(GUI::Handle page, const GUI::SAC::STROKE::Gesture &gesture)
  -> Flag {
  const Float west =
    GUI::NGA::GET::pan(page, VIEWS::ROLL::board().c_str()).x + ::railed();
  return gesture.from.at.x >= west;
}

}  // namespace

void SOUND::VIEWS::ROLL::easel() {
  const GUI::Handle page = document();
  ::tools(page);
  ::stated(page);
  if (!::drawing) return ::stroked(false);
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(page);
  if (!gesture.standing && !gesture.closed) return ::stroked(false);
  if (!::fielded(page, gesture)) return ::stroked(false);
  if (gesture.opened) {
    ::stroked(true);
    begun(page, gesture.from.cell);
  }
  const Flag rubbed =
    gesture.closed && ended(page, gesture.to.cell, gesture.dragged);
  if (!rubbed) painted(page, GUI::SAC::STROKE::GET::laid(page));
  if (gesture.closed) ::stroked(false);
}

void SOUND::VIEWS::ROLL::easel(SHELL::Session &session) {
  const STRING::Cold mode = GUI::SAC::STROKE::GET::mode(document());
  session.print(std::format(
    "easel mode {}", mode.empty() ? String("select") : String(mode)));
}

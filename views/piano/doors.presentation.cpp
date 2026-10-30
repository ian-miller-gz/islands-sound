// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../views.menu.marked.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot DOOR = "piano.bench.presentation";
constexpr STRING::Hot WHOSE = "piano.presentation";
constexpr STRING::Hot LINE = "bench presentation {}";
constexpr Whole COUNT = std::size(ROLL::PRESENTATIONS);
// The door is a square on the bench's first row, so it says its state by
// a glyph: the grid's cells, the staff's treble clef.
constexpr STRING::Hot GLYPHS[] = {"\u25A6", "\U0001D11E"};

void raise() {
  Vector<String> rows;
  Vector<Flag> ticks;
  for (Whole row = 0; row < ::COUNT; ++row) {
    rows.push_back(String(ROLL::PRESENTATIONS[row]));
    ticks.push_back(row == ROLL::GET::presentation());
  }
  VIEWS::MENU::raise(rows, ::WHOSE, ::DOOR, ticks);
}

}  // namespace

auto SOUND::VIEWS::ROLL::GET::pitched() -> Flag {
  return notation() == TONAL && presentation() == PRESENTATION::STAFF;
}

auto SOUND::VIEWS::ROLL::GET::staffed() -> Flag {
  return notation() == PERCUSSION || pitched();
}

void SOUND::VIEWS::ROLL::present() {
  const GUI::Handle page = document();
  const Whole row = MENU::taken(::WHOSE);
  if (row < ::COUNT) faced().presentation = row;
  if (GUI::GET::clicked(page, ::DOOR)) ::raise();
  GUI::set(page, ::DOOR, GUI::Text{String(::GLYPHS[GET::presentation()])});
}

void SOUND::VIEWS::ROLL::present(SHELL::Session &session) {
  if (!GET::staffed()) return;
  session.print(std::format(::LINE, PRESENTATIONS[GET::presentation()]));
}

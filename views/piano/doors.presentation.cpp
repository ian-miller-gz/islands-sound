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

void raise() {
  Vector<String> rows;
  Vector<Flag> ticks;
  for (Whole row = 0; row < ::COUNT; ++row) {
    rows.push_back(String(ROLL::PRESENTATIONS[row]));
    ticks.push_back(row == ROLL::GET::presentation());
  }
  VIEWS::MENU::raise(rows, ::WHOSE, ::DOOR, ticks);
}

auto struck() -> Flag { return ROLL::GET::notation() == ROLL::PERCUSSION; }

}  // namespace

void SOUND::VIEWS::ROLL::present() {
  const GUI::Handle page = document();
  const Whole row = MENU::taken(::WHOSE);
  if (row < ::COUNT) faced().presentation = row;
  if (GUI::GET::clicked(page, ::DOOR)) ::raise();
  const String word = PRESENTATIONS[GET::presentation()];
  GUI::set(page, ::DOOR, GUI::Text{word});
  GUI::set(page, ::DOOR, GUI::Visibility{::struck()});
}

void SOUND::VIEWS::ROLL::present(SHELL::Session &session) {
  if (!::struck()) return;
  session.print(std::format(::LINE, PRESENTATIONS[GET::presentation()]));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include <island/gui/nga.hpp>

#include "../../kind.hpp"
#include "../views.controls.hpp"
#include "../views.menu.marked.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot DOOR = "piano.bench.notation";
constexpr STRING::Hot WHOSE = "piano.notation";
constexpr STRING::Hot LINE = "bench notation {} map {}";
constexpr STRING::Hot APART = " ";
constexpr Whole COUNT = std::size(ROLL::NOTATIONS);

Whole pinned = NONE;

void raise() {
  Vector<String> rows;
  Vector<Flag> ticks;
  for (Whole row = 0; row < ::COUNT; ++row) {
    rows.push_back(String(ROLL::NOTATIONS[row]));
    ticks.push_back(row == ROLL::GET::notation());
  }
  VIEWS::MENU::raise(rows, ::WHOSE, ::DOOR, ticks);
}

void shown(GUI::Handle page, Whole notation) {
  const Flag tonal = notation == ROLL::TONAL;
  GUI::set(page, ROLL::TONIC, GUI::Visibility{tonal});
  GUI::set(page, ROLL::SCALE, GUI::Visibility{tonal});
  GUI::set(page, ROLL::MAP, GUI::Visibility{!tonal});
}

auto joined(const Vector<String> &names) -> String {
  String said;
  for (const String &name : names) said += (said.empty() ? "" : ::APART) + name;
  return said.empty() ? String(ROLL::ABSENT) : said;
}

auto hosted() -> const VIEWS::View * {
  for (const VIEWS::View &unit : VIEWS::roster())
    if (&VIEWS::GET::face(unit) == &ROLL::face()) return &unit;
  return nullptr;
}

void settle(Whole notation) {
  const VIEWS::View *unit = ::hosted();
  if (notation == ::pinned || unit == nullptr) return;
  const GUI::NGA::Zoom zoom =
    GUI::NGA::GET::zoom(VIEWS::document(), ROLL::board().c_str());
  if (VIEWS::zoom(*unit, zoom.value, zoom.down)) ::pinned = notation;
}

}  // namespace

auto SOUND::VIEWS::ROLL::faced() -> LANE::Face & {
  return LANE::faced(LANE::edited(KIND::NOTES));
}

auto SOUND::VIEWS::ROLL::GET::notation() -> Whole {
  const Whole notation = faced().notation;
  return notation < ::COUNT ? notation : TONAL;
}

void SOUND::VIEWS::ROLL::notate() {
  const GUI::Handle page = document();
  const Whole row = MENU::taken(::WHOSE);
  if (row < ::COUNT) faced().notation = row;
  if (GUI::GET::clicked(page, ::DOOR)) ::raise();
  const Whole notation = GET::notation();
  GUI::set(page, ::DOOR, GUI::Text{String(NOTATIONS[notation])});
  ::shown(page, notation);
  ::settle(notation);
  map();
  present();
}

void SOUND::VIEWS::ROLL::notate(SHELL::Session &session) {
  session.print(
    std::format(::LINE, NOTATIONS[GET::notation()], ::joined(GET::mapped())));
  present(session);
}

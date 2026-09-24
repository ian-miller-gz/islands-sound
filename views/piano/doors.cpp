// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../score.hpp"
#include "../../transport.hpp"
#include "../views.internal.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot TONIC = "piano.bench.tonic";
constexpr STRING::Hot SCALE = "piano.bench.scale";
constexpr STRING::Hot TONICS = "piano.tonic";
constexpr STRING::Hot SCALES = "piano.scale";
constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot FIRST = "major";

auto standing(const Key &key) -> Flag {
  return SCORE::degree(key, key.tonic) != NONE;
}

auto section() -> Key { return TRANSPORT::keyed(VIEWS::ROLL::marked()); }

void raise(STRING::Hot door, STRING::Hot whose, const Vector<String> &rows) {
  VIEWS::MENU::raise(rows, whose, door);
}

void tonics(GUI::Handle page) {
  const Whole row = VIEWS::MENU::taken(::TONICS);
  if (row != NONE && row < CLASSES) {
    const Key held = ::section();
    TRANSPORT::key(
      held.at, row, ::standing(held) ? held.scale : String(::FIRST));
  }
  if (!GUI::GET::clicked(page, ::TONIC)) return;
  Vector<String> rows;
  for (Whole tonic = 0; tonic < CLASSES; ++tonic)
    rows.push_back(String(SCORE::classed(tonic)));
  ::raise(::TONIC, ::TONICS, rows);
}

void scales(GUI::Handle page) {
  const Vector<STRING::Hot> names = SCORE::scales();
  const Whole row = VIEWS::MENU::taken(::SCALES);
  if (row != NONE && row <= names.size()) {
    const Key held = ::section();
    TRANSPORT::key(
      held.at, held.tonic, row < names.size() ? String(names[row]) : String());
  }
  if (!GUI::GET::clicked(page, ::SCALE)) return;
  Vector<String> rows;
  for (STRING::Hot name : names) rows.push_back(String(name));
  rows.push_back(String(::RESTING));
  ::raise(::SCALE, ::SCALES, rows);
}

}  // namespace

void SOUND::VIEWS::ROLL::doors() {
  const GUI::Handle page = document();
  ::tonics(page);
  ::scales(page);
  const Key held = ::section();
  GUI::set(page, ::TONIC, GUI::Text{String(SCORE::classed(held.tonic))});
  GUI::set(
    page, ::SCALE,
    GUI::Text{::standing(held) ? held.scale : String(::RESTING)});
}

void SOUND::VIEWS::ROLL::doors(SHELL::Session &session) {
  const GUI::Handle page = document();
  session.print(std::format(
    "bench key {} {}", String(GUI::GET::text(page, ::TONIC)),
    String(GUI::GET::text(page, ::SCALE))));
}

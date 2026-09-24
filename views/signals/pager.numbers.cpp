// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../kind.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "lane.number.name";
constexpr STRING::Hot BAND = "signals.tools";
constexpr STRING::Hot PANEL = "lane.numbers";
constexpr STRING::Hot ROWS = "lane.numbers.rows";
constexpr STRING::Hot DIGIT = "number";
constexpr STRING::Hot NAME = "name";
constexpr STRING::Hot NOTHING = "none";
constexpr Float WIDE = 200.0f;
constexpr Float FRAME = 12.0f;
constexpr Float AIR = 4.0f;
constexpr Whole WINDOW = 12;

Vector<VIEWS::SIGNALS::Word> run;

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL);
}

auto marked(Whole paged) -> Whole {
  for (Whole row = 0; row < ::run.size(); ++row)
    if (::run[row].number == paged) return row;
  return 0;
}

void seated(GUI::Handle page) {
  const GUI::Position at = GUI::GET::origin(page, ::DOOR);
  const GUI::Position home = GUI::GET::origin(page, ::BAND);
  const Float foot = GUI::GET::measured(page, ::DOOR).h + ::AIR;
  GUI::set(page, ::PANEL, GUI::Position{0.0f, at.y - home.y + foot});
}

void plate(GUI::Handle page) {
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  const Float deep = Float(std::min(::run.size(), ::WINDOW)) * step;
  GUI::set(page, ::PANEL, GUI::Extent{::WIDE, ::FRAME + deep});
  ::seated(page);
  const Vector<Whole> shown = VIEWS::SIGNALS::seats(::ROWS, ::run.size());
  for (Whole seat = 0; seat < shown.size(); ++seat) {
    const String cell = std::format("{}.{}", ::ROWS, seat);
    const VIEWS::SIGNALS::Word &word = ::run[shown[seat]];
    GUI::set(
      page, (cell + "." + ::DIGIT).c_str(),
      GUI::Text{std::to_string(word.number)});
    GUI::set(page, (cell + "." + ::NAME).c_str(), GUI::Text{word.name});
  }
}

void spread(GUI::Handle page, Flag on) {
  if (on) ::run = VIEWS::SIGNALS::worded();
  GUI::set(page, ::PANEL, GUI::Visibility{on});
  if (!on) return;
  ::plate(page);
  GUI::set(page, ::ROWS, GUI::Cursor{::marked(VIEWS::SIGNALS::number())});
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= ::run.size()) return;
  VIEWS::SIGNALS::number(::run[row].number);
  ::spread(page, false);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::numbers() {
  const GUI::Handle page = document();
  if (paged() != KIND::CONTROL) return ::spread(page, false);
  if (GUI::GET::clicked(page, ::DOOR)) return ::spread(page, !::standing(page));
  if (!::standing(page)) return;
  ::plate(page);
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page);
  if (elsewhere(::DOOR, ::PANEL)) ::spread(page, false);
}

void SOUND::VIEWS::SIGNALS::numbers(SHELL::Session &session) {
  const GUI::Handle page = document();
  if (!::standing(page)) return;
  session.print(std::format(
    "numbers rows {} cursor {} map {}", ::run.size(),
    GUI::GET::cursor(page, ::ROWS),
    mapped().empty() ? String(::NOTHING) : mapped()));
  for (const Word &word : ::run)
    session.print(std::format(
      "word {} {}", word.number,
      word.name.empty() ? String(::NOTHING) : word.name));
}

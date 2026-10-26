// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../kind.hpp"
#include "signals.mapping.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUTTON = "lane.map";
constexpr STRING::Hot PANEL = "lane.mapping";
constexpr STRING::Hot ROWS = "lane.mapping.rows";
constexpr STRING::Hot NOTHING = "none";
constexpr Whole HEAD = 0;

Vector<VIEWS::SIGNALS::Offer> rows;
Flag choosing = false;

void spread(GUI::Handle page, Flag on) {
  ::choosing = false;
  if (on) ::rows = VIEWS::SIGNALS::reached();
  GUI::set(page, ::PANEL, GUI::Visibility{on});
  if (on) VIEWS::SIGNALS::hang(::rows);
}

void browse(GUI::Handle page) {
  ::choosing = true;
  ::rows = VIEWS::SIGNALS::offered();
  GUI::set(page, ::ROWS, GUI::Scroll{0});
  GUI::set(page, ::ROWS, GUI::Cursor{0});
  VIEWS::SIGNALS::hang(::rows);
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= ::rows.size()) return;
  if (!::choosing && row + 1 == ::rows.size()) return ::browse(page);
  if (::rows[row].words == 0) return;
  VIEWS::SIGNALS::map(
    !::choosing && row == ::HEAD ? String() : ::rows[row].name);
  ::spread(page, false);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::mapping() {
  if (paged() == KIND::LOGIC) return aiming();
  const GUI::Handle page = document();
  const Flag numbered = paged() == KIND::CONTROL;
  GUI::set(page, ::BUTTON, GUI::Visibility{numbered});
  if (!numbered) return ::spread(page, false);
  if (GUI::GET::clicked(page, ::BUTTON)) return ::spread(page, !hung());
  if (!hung()) return;
  hang(::rows);
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page);
  if (elsewhere(::BUTTON, ::PANEL)) ::spread(page, false);
}

void SOUND::VIEWS::SIGNALS::mapping(SHELL::Session &session) {
  if (paged() == KIND::LOGIC) return aiming(session);
  const GUI::Handle page = document();
  if (!hung()) return;
  const ARRANGEMENT::TRACK::Lane *lane = edited();
  const Flag bare = lane == nullptr || lane->map.empty();
  session.print(std::format(
    "mapping {} rows {} cursor {} map {}", ::choosing ? "catalog" : "wired",
    ::rows.size(), GUI::GET::cursor(page, ::ROWS),
    bare ? String(::NOTHING) : lane->map));
  for (const Offer &offer : ::rows)
    session.print(std::format(
      "choice {} params {} {}", offer.name,
      offer.words == NONE ? String(::NOTHING) : std::to_string(offer.words),
      offer.words == 0 ? "greyed" : "open"));
}

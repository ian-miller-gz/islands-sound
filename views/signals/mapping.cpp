// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUTTON = "lane.map";
constexpr STRING::Hot BAND = "signals.tools";
constexpr STRING::Hot PANEL = "lane.mapping";
constexpr STRING::Hot ROWS = "lane.mapping.rows";
constexpr STRING::Hot ACT = "map";
constexpr STRING::Hot NOTHING = "none";
constexpr Float WIDE = 220.0f;
constexpr Float FRAME = 12.0f;
constexpr Float AIR = 4.0f;
constexpr Whole WINDOW = 12;
constexpr Whole HEAD = 0;

Vector<VIEWS::SIGNALS::Offer> rows;
Flag choosing = false;

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL);
}

void seated(GUI::Handle page) {
  const GUI::Position at = GUI::GET::origin(page, ::BUTTON);
  const GUI::Position home = GUI::GET::origin(page, ::BAND);
  const Float foot = GUI::GET::measured(page, ::BUTTON).h + ::AIR;
  GUI::set(page, ::PANEL, GUI::Position{0.0f, at.y - home.y + foot});
}

void plate(GUI::Handle page) {
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  const Float deep = Float(std::min(::rows.size(), ::WINDOW)) * step;
  GUI::set(page, ::PANEL, GUI::Extent{::WIDE, ::FRAME + deep});
  ::seated(page);
  VIEWS::SIGNALS::listing(::ROWS, ::rows);
}

void spread(GUI::Handle page, Flag on) {
  ::choosing = false;
  if (on) ::rows = VIEWS::SIGNALS::reached();
  GUI::set(page, ::PANEL, GUI::Visibility{on});
  if (on) ::plate(page);
}

void browse(GUI::Handle page) {
  ::choosing = true;
  ::rows = VIEWS::SIGNALS::offered();
  GUI::set(page, ::ROWS, GUI::Scroll{0});
  GUI::set(page, ::ROWS, GUI::Cursor{0});
  ::plate(page);
}

void wrote(const String &plugin) {
  const Whole track = VIEWS::SIGNALS::attended();
  const Whole lane = VIEWS::SIGNALS::laned();
  if (track == NONE || lane == NONE) return;
  const String stood = TIMELINE::held().tracks[track].lanes[lane].map;
  if (!TIMELINE::map(track, lane, plugin)) return;
  HISTORY::record(
    {.act = ::ACT,
     .maps = {{.track = track, .lane = lane, .map = plugin, .stood = stood}}});
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= ::rows.size()) return;
  if (!::choosing && row + 1 == ::rows.size()) return ::browse(page);
  if (::rows[row].words == 0) return;
  ::wrote(!::choosing && row == ::HEAD ? String() : ::rows[row].name);
  ::spread(page, false);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::mapping() {
  const GUI::Handle page = document();
  const Flag numbered = paged() == KIND::CONTROL;
  GUI::set(page, ::BUTTON, GUI::Visibility{numbered});
  if (!numbered) return ::spread(page, false);
  if (GUI::GET::clicked(page, ::BUTTON))
    return ::spread(page, !::standing(page));
  if (!::standing(page)) return;
  ::plate(page);
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page);
  if (elsewhere(::BUTTON, ::PANEL)) ::spread(page, false);
}

void SOUND::VIEWS::SIGNALS::mapping(SHELL::Session &session) {
  const GUI::Handle page = document();
  if (!::standing(page)) return;
  const Whole track = attended(), lane = laned();
  const String map = track == NONE || lane == NONE
                       ? String()
                       : TIMELINE::held().tracks[track].lanes[lane].map;
  session.print(std::format(
    "mapping {} rows {} cursor {} map {}", ::choosing ? "catalog" : "wired",
    ::rows.size(), GUI::GET::cursor(page, ::ROWS),
    map.empty() ? String(::NOTHING) : map));
  for (const Offer &offer : ::rows)
    session.print(std::format(
      "choice {} params {} {}", offer.name,
      offer.words == NONE ? String(::NOTHING) : std::to_string(offer.words),
      offer.words == 0 ? "greyed" : "open"));
}

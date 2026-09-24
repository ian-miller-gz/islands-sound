// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../commands.hpp"
#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;

constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot KIND = "kind";
constexpr STRING::Hot EDIT = "edit";
constexpr STRING::Hot SHED = "shed";
constexpr STRING::Hot SHOWN = "shown";
constexpr STRING::Hot HIDDEN = "hidden";
constexpr STRING::Hot KINDS = "trace.kind";

Flag laned = false;
Whole adding = KIND::AUDIO;

auto admits(Whole track, Whole lane) -> Flag {
  const String root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) && GRAPH::admitted(root, lane) != NONE;
}

void grown(GUI::Handle page, Whole track) {
  if (!GUI::GET::clicked(page, TRACE::GROW)) return;
  COMMANDS::laned(track, ::adding);
}

auto acted(GUI::Handle page, const String &cell, Whole track, Whole lane)
  -> Flag {
  if (GUI::GET::clicked(page, (cell + "." + ::EDIT).c_str())) {
    TRACE::edit(track, lane);
    return false;
  }
  if (!GUI::GET::clicked(page, (cell + "." + ::SHED).c_str())) return false;
  return COMMANDS::unlaned(track, lane);
}

void rowed(GUI::Handle page, Whole track) {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  GUI::set(page, TRACE::LANES, GUI::Rows{lanes.size()});
  const Whole first = GUI::GET::first(page, TRACE::LANES);
  for (Whole row = 0; first + row < lanes.size(); ++row) {
    const String cell = std::format("{}.{}", TRACE::LANES, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    if (::acted(page, cell, track, first + row)) return;
    GUI::set(
      page, (cell + "." + ::NAMED).c_str(), GUI::Text{lanes[first + row].name});
    GUI::set(
      page, (cell + "." + ::KIND).c_str(),
      GUI::Text{KIND::spoken(lanes[first + row].kind)});
  }
}

}  // namespace

auto SOUND::VIEWS::TRACE::laning() -> Flag { return ::laned; }
void SOUND::VIEWS::TRACE::laning(Flag on) { ::laned = on; }

auto SOUND::VIEWS::TRACE::kinded(
  GUI::Handle page, STRING::Hot door, STRING::Hot whose, Whole &kind) -> Flag {
  const Whole row = VIEWS::MENU::taken(whose);
  if (row != NONE && row <= KIND::DATA) kind = row;
  if (GUI::GET::clicked(page, door)) {
    Vector<String> rows;
    for (Whole each = KIND::AUDIO; each <= KIND::DATA; ++each)
      rows.push_back(String(KIND::spoken(each)));
    VIEWS::MENU::raise(rows, whose, door);
  }
  GUI::set(page, door, GUI::Text{KIND::spoken(kind)});
  return row != NONE;
}

void SOUND::VIEWS::TRACE::lanes() {
  const GUI::Handle page = document();
  const Whole track = attended();
  kinded(page, KINDED, ::KINDS, ::adding);
  if (track == NONE) {
    GUI::set(page, LANES, GUI::Rows{0});
    return;
  }
  ::rowed(page, track);
  ::grown(page, track);
}

void SOUND::VIEWS::TRACE::lanes(SHELL::Session &session) {
  const Whole track = attended();
  const Whole count =
    track == NONE ? 0 : TIMELINE::held().tracks[track].lanes.size();
  session.print(std::format(
    "lanes {} rows {} kind {}", ::laned ? ::SHOWN : ::HIDDEN, count,
    KIND::spoken(::adding)));
  for (Whole lane = 0; lane < count; ++lane) {
    const ARRANGEMENT::TRACK::Lane &held =
      TIMELINE::held().tracks[track].lanes[lane];
    session.print(std::format(
      "lane {} {} {} admits {}", lane, held.name, KIND::spoken(held.kind),
      ::admits(track, lane) ? "on" : "off"));
  }
}

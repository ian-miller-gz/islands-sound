// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../commands.hpp"
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
constexpr STRING::Hot KINDS = "trace.kind";

Whole adding = KIND::AUDIO;

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

auto SOUND::VIEWS::TRACE::adding() -> Whole { return ::adding; }

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
  asked(page);
  marked(page);
}

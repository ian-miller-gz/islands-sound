// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../commands.hpp"
#include "../../timeline.hpp"
#include "tracks.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MAKE = "tracks.make";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot EDIT = "edit";
constexpr STRING::Hot SHED = "shed";

void named(GUI::Handle page, const String &cell, Whole track) {
  GUI::set(page, cell.c_str(), GUI::Text{TIMELINE::held().tracks[track].name});
}

auto acted(GUI::Handle page, const String &cell, Whole track) -> Flag {
  if (GUI::GET::clicked(page, (cell + "." + ::EDIT).c_str())) {
    VIEWS::TRACKS::edit(track);
    return false;
  }
  if (!GUI::GET::clicked(page, (cell + "." + ::SHED).c_str())) return false;
  COMMANDS::dropped(track);
  return true;
}

void run() {
  const GUI::Handle page = VIEWS::document();
  if (GUI::GET::clicked(page, ::MAKE))
    COMMANDS::track(std::format("track{}", TIMELINE::held().tracks.size() + 1));
  VIEWS::TRACKS::editing();
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  GUI::set(page, VIEWS::TRACKS::ROWS, GUI::Rows{tracks.size()});
  const Whole first = GUI::GET::first(page, VIEWS::TRACKS::ROWS);
  for (Whole row = 0; first + row < tracks.size(); ++row) {
    const String cell = std::format("{}.{}", VIEWS::TRACKS::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    if (::acted(page, cell, first + row)) return;
    ::named(page, cell + "." + ::NAMED, first + row);
  }
}

void state(SHELL::Session &session) {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  session.print(std::format(
    "tracks rows {} cursor {} split {:g}", tracks.size(),
    GUI::GET::cursor(VIEWS::document(), VIEWS::TRACKS::ROWS), VIEWS::split()));
  for (Whole row = 0; row < tracks.size(); ++row)
    session.print(std::format(
      "row {} {} lanes {}{}", row, tracks[row].name, tracks[row].lanes.size(),
      tracks[row].bus ? " bus" : ""));
  VIEWS::TRACKS::editing(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = VIEWS::TRACKS::NAME, .panel = true, .run = ::run, .state = ::state});

}  // namespace

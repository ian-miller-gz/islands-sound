// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../plugin.hpp"
#include "views.internal.hpp"
#include "views.scan.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "deck.scan";
constexpr STRING::Hot ASK = "scan.ask";
constexpr STRING::Hot PLATE = "scan.";
constexpr STRING::Hot TAKE = "scan.take";
constexpr STRING::Hot CLOSE = "scan.close";
constexpr STRING::Hot NOTE = "scan.note";
constexpr STRING::Hot ROWS = "scan.rows";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot PATHED = "path";
constexpr STRING::Hot UNSCANNED = "No scan yet";
constexpr STRING::Hot SUMMED = "{} roots, {} vst3 found";

void raised(GUI::Handle page, Flag on) {
  GUI::set(page, ::ASK, GUI::Visibility{on});
}

void worded(GUI::Handle page, const PLUGIN::SCAN::Report &report) {
  GUI::set(
    page, ::NOTE,
    GUI::Text{
      report.runs == 0
        ? String(::UNSCANNED)
        : std::format(::SUMMED, report.roots.size(), report.found.size())});
}

void rowed(GUI::Handle page, const PLUGIN::SCAN::Report &report) {
  const Vector<PLUGIN::SCAN::Found> &rows = report.found;
  GUI::set(page, ::ROWS, GUI::Rows{rows.size()});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const PLUGIN::SCAN::Found &one = rows[first + row];
    GUI::set(page, (cell + "." + ::NAMED).c_str(), GUI::Text{one.name});
    GUI::set(page, (cell + "." + ::PATHED).c_str(), GUI::Text{one.path});
  }
}

}  // namespace

void SOUND::VIEWS::scan() {
  const GUI::Handle page = document();
  const Flag standing = GUI::GET::visibility(page, ::ASK);
  if (GUI::GET::clicked(page, ::DOOR)) return ::raised(page, !standing);
  if (!standing) return;
  VIEWS::SCAN::roots(page);
  if (GUI::GET::clicked(page, ::TAKE)) PLUGIN::SCAN::run();
  ::worded(page, PLUGIN::SCAN::held());
  ::rowed(page, PLUGIN::SCAN::held());
  if (GUI::GET::clicked(page, ::CLOSE) || VIEWS::elsewhere(::DOOR, ::PLATE))
    ::raised(page, false);
}

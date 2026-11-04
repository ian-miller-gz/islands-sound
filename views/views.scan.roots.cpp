// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../plugin.hpp"
#include "views.internal.hpp"
#include "views.scan.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ROWS = "scan.roots";
constexpr STRING::Hot FIELD = "scan.path";
constexpr STRING::Hot ADD = "scan.add";
constexpr STRING::Hot PATHED = "path";
constexpr STRING::Hot SHED = "shed";

void added(GUI::Handle page) {
  if (!GUI::GET::committed(page, ::FIELD) && !GUI::GET::clicked(page, ::ADD))
    return;
  if (!PLUGIN::SCAN::add(GUI::GET::text(page, ::FIELD))) return;
  GUI::set(page, ::FIELD, GUI::Text{String()});
  GUI::set(page, ::FIELD, GUI::Caret{});
}

auto shed(GUI::Handle page, const String &cell, Whole row, Whole homes)
  -> Flag {
  if (row < homes) return false;
  if (!GUI::GET::clicked(page, (cell + "." + ::SHED).c_str())) return false;
  return PLUGIN::SCAN::drop(row - homes);
}

void rowed(GUI::Handle page, const Vector<String> &rows, Whole homes) {
  GUI::set(page, ::ROWS, GUI::Rows{rows.size()});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    if (::shed(page, cell, first + row, homes)) return;
    GUI::set(
      page, (cell + "." + ::PATHED).c_str(), GUI::Text{rows[first + row]});
    GUI::set(
      page, (cell + "." + ::SHED).c_str(),
      GUI::Visibility{first + row >= homes});
  }
}

}  // namespace

void SOUND::VIEWS::SCAN::roots(GUI::Handle page) {
  ::added(page);
  Vector<String> rows = PLUGIN::SCAN::homes();
  const Whole homes = rows.size();
  const Vector<String> &own = PLUGIN::SCAN::own();
  rows.insert(rows.end(), own.begin(), own.end());
  ::rowed(page, rows, homes);
}

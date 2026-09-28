// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../session.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PLUS = "graph.plus";
constexpr STRING::Hot CATALOG = "graph.catalog";
constexpr STRING::Hot ROWS = "graph.catalog.rows";
constexpr STRING::Hot BROWSE = "Browse…";
constexpr Whole QUICK = 8;
constexpr Float WIDE = 176.0f;
constexpr Float FRAME = 12.0f;

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::CATALOG);
}

void spread(GUI::Handle page, Flag on) {
  GUI::set(page, ::CATALOG, GUI::Visibility{on});
}

auto quick() -> Vector<String> {
  const Vector<String> plugins = GRAPH::offers();
  Vector<String> rows;
  for (const String &name : SESSION::held().recents)
    if (std::find(plugins.begin(), plugins.end(), name) != plugins.end())
      rows.push_back(name);
  if (rows.size() > ::QUICK) rows.resize(::QUICK);
  for (const String &name : plugins) {
    if (rows.size() >= ::QUICK) break;
    if (std::find(rows.begin(), rows.end(), name) == rows.end())
      rows.push_back(name);
  }
  rows.push_back(String(::BROWSE));
  return rows;
}

void offered(GUI::Handle page, const Vector<String> &rows) {
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  GUI::set(
    page, ::CATALOG, GUI::Extent{::WIDE, ::FRAME + Float(rows.size()) * step});
  GUI::set(page, ::ROWS, GUI::Rows{rows.size()});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    GUI::set(page, cell.c_str(), GUI::Text{rows[first + row]});
  }
}

void took(GUI::Handle page, const Vector<String> &rows) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= rows.size()) return;
  ::spread(page, false);
  if (row + 1 == rows.size()) return VIEWS::WIRED::raise(true);
  VIEWS::WIRED::take(rows[row]);
}

}  // namespace

void SOUND::VIEWS::WIRED::door() {
  if (browsing()) return;
  const GUI::Handle page = document();
  const Vector<String> rows = ::quick();
  ::offered(page, rows);
  if (GUI::GET::clicked(page, ::PLUS)) return ::spread(page, !::standing(page));
  if (!::standing(page)) return;
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page, rows);
  if (VIEWS::elsewhere(::PLUS, ::CATALOG)) ::spread(page, false);
}

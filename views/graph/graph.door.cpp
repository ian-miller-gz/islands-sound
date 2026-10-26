// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <island/gui/tip.hpp>

#include "../../graph.hpp"
#include "../../session.hpp"
#include "graph.keys.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PLUS = "graph.plus";
constexpr STRING::Hot CATALOG = "graph.catalog";
constexpr STRING::Hot ROWS = "graph.catalog.rows";
constexpr STRING::Hot BROWSE = "Browse…";
constexpr Whole QUICK = 8;
constexpr Float WIDE = 176.0f;
constexpr Float FRAME = 12.0f;
constexpr Float SIDES = 2.0f;

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

void written(GUI::Handle page, const String &cell, const String &whole) {
  const Float inset = GUI::GET::position(page, ::ROWS).x +
                      GUI::GET::extent(page, ::ROWS).w +
                      ::SIDES * GUI::GET::pad(page, cell.c_str());
  const String text = VIEWS::LETTERS::cropped(
    whole, ::WIDE - inset, GUI::GET::size(page, cell.c_str()));
  GUI::set(page, cell.c_str(), GUI::Text{text});
  GUI::TIP::note(page, cell.c_str(), text == whole ? String() : whole);
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
    ::written(page, cell, rows[first + row]);
  }
}

void took(GUI::Handle page, const Vector<String> &rows) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= rows.size()) return;
  VIEWS::WIRED::BROWSE::quick(false);
  if (row + 1 == rows.size()) return VIEWS::WIRED::raise(true);
  VIEWS::WIRED::take(rows[row], ::ROWS, row);
}

}  // namespace

void SOUND::VIEWS::WIRED::door() {
  if (browsing()) return;
  const GUI::Handle page = document();
  const Vector<String> rows = ::quick();
  ::offered(page, rows);
  if (GUI::GET::clicked(page, ::PLUS)) return BROWSE::quick(!BROWSE::quicked());
  if (!BROWSE::quicked()) return;
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page, rows);
  if (VIEWS::elsewhere(::PLUS, ::CATALOG)) BROWSE::quick(false);
}

void SOUND::VIEWS::WIRED::BROWSE::quick(Flag on) {
  GUI::set(VIEWS::document(), ::CATALOG, GUI::Visibility{on});
}

auto SOUND::VIEWS::WIRED::BROWSE::quicked() -> Flag {
  return GUI::GET::visibility(VIEWS::document(), ::CATALOG);
}

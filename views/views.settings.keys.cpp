// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <utility>

#include "views.hotkeys.hpp"

namespace {
using namespace SOUND;
namespace HOTKEYS = SOUND::VIEWS::HOTKEYS;

constexpr STRING::Hot LIST = "settings.keys";
constexpr STRING::Hot THEME = "settings.theme.tab";
constexpr STRING::Hot LISTING = "settings.keys.tab";
constexpr STRING::Hot THEMED[] = {
  "settings.themed", "settings.own", "settings.shade"};
constexpr STRING::Hot PLATE = "{}.plate";
constexpr STRING::Hot ROW = "{}.{}";
constexpr STRING::Hot CELL = "{}.{}.{}";
constexpr STRING::Hot FAMILY = "family", ACT = "act", CHORD = "chord";
struct Dress {
  STRING::Hot plate = "", label = "";
};

constexpr Dress RESTING = {"tab", ""};
constexpr Dress STANDING = {"standing", ""};

Flag keyed = false;

auto rows() -> const Vector<HOTKEYS::Row> & {
  static const Vector<HOTKEYS::Row> listed = HOTKEYS::listed();
  return listed;
}

void tabbed(GUI::Handle page, STRING::Hot tab, Flag standing) {
  const Dress &dress = standing ? ::STANDING : ::RESTING;
  const String plate = std::format(::PLATE, tab);
  GUI::set(page, plate.c_str(), GUI::Style{dress.plate});
  GUI::set(page, tab, GUI::Style{dress.label});
}

void lettered(GUI::Handle page, Whole cell, STRING::Hot part, String text) {
  const String id = std::format(::CELL, ::LIST, cell, part);
  GUI::set(page, id.c_str(), GUI::Text{std::move(text)});
}

void written(GUI::Handle page) {
  const Vector<HOTKEYS::Row> &listed = ::rows();
  const Whole first = GUI::GET::first(page, ::LIST);
  for (Whole cell = 0; first + cell < listed.size(); ++cell) {
    const String id = std::format(::ROW, ::LIST, cell);
    if (!GUI::GET::visibility(page, id.c_str())) break;
    const HOTKEYS::Row &row = listed[first + cell];
    ::lettered(page, cell, ::FAMILY, row.family);
    ::lettered(page, cell, ::ACT, row.act);
    ::lettered(page, cell, ::CHORD, HOTKEYS::spelled(row.chord));
  }
}

}  // namespace

void SOUND::VIEWS::HOTKEYS::paged(GUI::Handle page, Flag keys) {
  ::keyed = keys;
  ::tabbed(page, ::THEME, !keys);
  ::tabbed(page, ::LISTING, keys);
  for (STRING::Hot id : ::THEMED) GUI::set(page, id, GUI::Visibility{!keys});
  GUI::set(page, ::LIST, GUI::Visibility{keys});
  if (keys) GUI::set(page, ::LIST, GUI::Rows{::rows().size()});
}

void SOUND::VIEWS::HOTKEYS::turned(GUI::Handle page) {
  if (GUI::GET::clicked(page, ::THEME)) paged(page, false);
  if (GUI::GET::clicked(page, ::LISTING)) paged(page, true);
  if (::keyed) ::written(page);
}

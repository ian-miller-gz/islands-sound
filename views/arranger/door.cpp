// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;

constexpr Float WIDE = 176.0f;
constexpr Float FRAME = 12.0f;

const String DOOR = VIEWS::icon(NAME) + ".door";
const String PANEL = VIEWS::icon(NAME) + ".pages";
const String ROWS = ::PANEL + ".rows";

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL.c_str());
}

void plate(GUI::Handle page) {
  const Float step =
    GUI::GET::pitch(page, ::ROWS.c_str()) + GUI::GET::pad(page, ::ROWS.c_str());
  GUI::set(
    page, ::PANEL.c_str(), GUI::Extent{::WIDE, ::FRAME + Float(COUNT) * step});
  GUI::set(page, ::ROWS.c_str(), GUI::Rows{COUNT});
  for (Whole row = 0; row < COUNT; ++row)
    GUI::set(
      page, std::format("{}.{}", ::ROWS, row).c_str(),
      GUI::Text{PAGES[row].label});
}

void spread(GUI::Handle page, Flag on) {
  GUI::set(page, ::PANEL.c_str(), GUI::Visibility{on});
  if (!on) return;
  ::plate(page);
  GUI::set(page, ::ROWS.c_str(), GUI::Cursor{turned()});
}

void took(GUI::Handle page) {
  if (!turned(GUI::GET::cursor(page, ::ROWS.c_str()))) return;
  VIEWS::choose(NAME);
  ::spread(page, false);
}

}  // namespace

void SOUND::VIEWS::ARRANGER::door() {
  const GUI::Handle page = document();
  if (GUI::GET::clicked(page, ::DOOR.c_str()))
    return ::spread(page, !::standing(page));
  if (!::standing(page)) return;
  ::plate(page);
  if (GUI::GET::activated(page, ::ROWS.c_str())) return ::took(page);
  if (elsewhere(::DOOR.c_str(), ::PANEL.c_str())) ::spread(page, false);
}

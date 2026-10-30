// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.internal.hpp"
#include "views.lit.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HINT = ".hint";
constexpr STRING::Hot RESTING = "handle";
constexpr STRING::Hot LIT = "handlelit";
constexpr Whole REST = 6;

Whole born = 0, rested = 0;
STRING::Hot toggled = nullptr, handled = nullptr;

auto icon(const VIEWS::View &unit) -> String {
  return String(VIEWS::SPLIT::BAR) + "." + unit.name;
}

void worded(GUI::Handle page, Flag fresh) {
  if (!fresh) return;
  for (const VIEWS::View &unit : VIEWS::roster()) {
    if (unit.panel) continue;
    const String strip = VIEWS::icon(unit.name) + ::HINT;
    const String band = ::icon(unit) + ::HINT;
    GUI::set(
      page, band.c_str(), GUI::Text{GUI::GET::text(page, strip.c_str())});
  }
}

void worn(
  GUI::Handle page, STRING::Hot id, STRING::Hot style, STRING::Hot &kept,
  Flag fresh) {
  if (!fresh && kept == style) return;
  GUI::set(page, id, GUI::Style{style});
  kept = style;
}

auto lit(GUI::Handle page) -> Flag {
  const Flag over = String(GUI::GET::hover(page)) == VIEWS::SPLIT::BAR;
  ::rested = over ? std::min(::rested + 1, ::REST) : 0;
  const Flag pressed = over && GUI::GET::pressed(page);
  return VIEWS::SPLIT::held() || pressed || ::rested >= ::REST;
}

void dressed(GUI::Handle page, Flag fresh) {
  const STRING::Hot mark = VIEWS::SPLIT::open() ? VIEWS::MARK : VIEWS::PLAIN;
  ::worn(page, VIEWS::SPLIT::TOGGLE, mark, ::toggled, fresh);
  const STRING::Hot ink = ::lit(page) ? ::LIT : ::RESTING;
  ::worn(page, VIEWS::SPLIT::HANDLE, ink, ::handled, fresh);
}

}  // namespace

auto SOUND::VIEWS::SPLIT::mark(Whole region) -> STRING::Hot {
  return focused() == region ? MARK : DIM;
}

void SOUND::VIEWS::SPLIT::chose() {
  const GUI::Handle page = sheet();
  const Vector<View> &units = roster();
  for (Whole at = 0; at < units.size(); ++at) {
    if (units[at].panel || units[at].door != nullptr) continue;
    const String id = ::icon(units[at]);
    if (GUI::GET::clicked(page, id.c_str())) choose(units[at].name);
    light(page, id, at == lower() ? mark(BELOW) : PLAIN);
  }
  const Flag fresh = reborn(::born);
  ::worded(page, fresh);
  ::dressed(page, fresh);
}

auto SOUND::VIEWS::SPLIT::GET::handle() -> STRING::Hot {
  return ::handled == nullptr ? ::RESTING : ::handled;
}

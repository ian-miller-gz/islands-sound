// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using INPUT::KEYS::HOTKEY;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr Whole CYCLE = 6, TAKE = 8;
constexpr Whole PANEL = 'J';
constexpr Whole FIRST = '1', SECOND = '2', THIRD = '3', FOURTH = '4';

Flag hold = false;
Float before = VIEWS::SPLIT::SHUT;
String asked;

struct Key {
  VIEWS::Chord chord;
  void (*act)(Whole code) = nullptr;
};

void cycled(Whole) {
  if (!VIEWS::SPLIT::open()) return;
  VIEWS::SPLIT::focus(VIEWS::SPLIT::focused() == UPPER ? BELOW : UPPER);
}

void taken(Whole) {
  if (!::hold) VIEWS::SPLIT::HOLD::take();
  ::hold = true;
}

void toggled(Whole) { VIEWS::SPLIT::toggle(); }

void widened(Whole) {
  const Float room = VIEWS::SPLIT::measured(VIEWS::sheet()).room;
  if (VIEWS::SPLIT::height() >= room) return VIEWS::SPLIT::seat(::before);
  ::before = VIEWS::SPLIT::height();
  VIEWS::SPLIT::seat(room);
}

auto icon(const VIEWS::View &unit, Whole region) -> String {
  if (region == UPPER) return VIEWS::icon(unit.name);
  return String(VIEWS::SPLIT::BAR) + VIEWS::SPLIT::DOT + unit.name;
}

void picked(const VIEWS::View &unit, const String &id, Whole region) {
  if (unit.door != nullptr)
    ::asked = id;
  else if (region == BELOW)
    VIEWS::SPLIT::choose(unit.name);
  else
    VIEWS::choose(unit.name);
}

void chosen(Whole code) {
  const Whole region = VIEWS::SPLIT::focused();
  Whole slot = code - ::FIRST;
  for (const VIEWS::View &unit : VIEWS::roster()) {
    if (unit.panel) continue;
    const String id = ::icon(unit, region);
    if (slot-- == 0) return ::picked(unit, id, region);
    if (unit.door != nullptr && slot-- == 0)
      return ::picked(unit, id + VIEWS::SPLIT::DOOR, region);
  }
}

constexpr Key KEYS[] = {
  {{.action = HOTKEY, .code = ::CYCLE}, ::cycled},
  {{.action = HOTKEY, .code = ::TAKE}, ::taken},
  {{.code = ::PANEL, .control = true}, ::toggled},
  {{.code = ::FIRST, .control = true}, ::chosen},
  {{.code = ::SECOND, .control = true}, ::chosen},
  {{.code = ::THIRD, .control = true}, ::chosen},
  {{.code = ::FOURTH, .control = true}, ::chosen},
  {{.code = ::PANEL, .control = true, .shift = true}, ::widened}};

}  // namespace

void SOUND::VIEWS::SPLIT::keyed() {
  ::asked.clear();
  for (const ::Key &row : ::KEYS)
    if (chorded(row.chord)) row.act(row.chord.code);
  if (::hold) ::hold = HOLD::keyed();
  HOLD::shown(::hold);
}

auto SOUND::VIEWS::SPLIT::held() -> Flag { return ::hold; }

auto SOUND::VIEWS::SPLIT::struck(const String &id) -> Flag {
  return !::asked.empty() && ::asked == id;
}

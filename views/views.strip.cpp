// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

auto SOUND::VIEWS::icon(STRING::Hot view) -> String {
  return String(STRIP) + "." + view;
}

namespace {
using namespace SOUND;

constexpr STRING::Hot DOORED = "door";
constexpr STRING::Hot PAIRED = "set";
constexpr STRING::Hot RESTING = "pair";
constexpr STRING::Hot LIT = "pairlit";

void paired(const VIEWS::View &unit, Flag standing) {
  const GUI::Handle page = VIEWS::sheet();
  const String icon = VIEWS::icon(unit.name);
  const String door = icon + "." + ::DOORED;
  const String plate = icon + "." + ::PAIRED;
  GUI::set(
    page, door.c_str(), GUI::Style{standing ? VIEWS::MARK : VIEWS::PLAIN});
  const String over = String(GUI::GET::hover(page));
  const Flag rested = over == icon || over == door;
  GUI::set(
    page, plate.c_str(),
    GUI::Style{
      standing ? VIEWS::MARK
      : rested ? ::LIT
               : ::RESTING});
}

}  // namespace

void SOUND::VIEWS::shed(const View &unit) {
  if (unit.shed != nullptr) unit.shed();
}

void SOUND::VIEWS::show(const View &unit, Flag on) {
  GUI::set(sheet(), unit.name, GUI::Visibility{on});
  if (!on) shed(unit);
}

void SOUND::VIEWS::show() {
  const Vector<View> &units = registry();
  for (Whole row = 0; row < units.size(); ++row) {
    const Flag on = units[row].panel ? standings()[row] : row == chosen();
    show(units[row], on);
    if (units[row].panel) continue;
    GUI::set(
      sheet(), icon(units[row].name).c_str(),
      GUI::Style{row == chosen() ? MARK : PLAIN});
    if (units[row].door != nullptr) ::paired(units[row], row == chosen());
  }
}

void SOUND::VIEWS::doors() {
  for (const View &unit : registry())
    if (unit.door != nullptr) unit.door();
}

void SOUND::VIEWS::chose() {
  const Vector<View> &units = registry();
  for (Whole row = 0; row < units.size(); ++row)
    if (
      !units[row].panel &&
      GUI::GET::clicked(sheet(), icon(units[row].name).c_str()))
      choose(units[row].name);
}

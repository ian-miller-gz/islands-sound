// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.lit.hpp"
#include "views.split.hpp"

auto SOUND::VIEWS::icon(STRING::Hot view) -> String {
  return String(STRIP) + "." + view;
}

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
    const Flag on = units[row].panel ? standings()[row]
                                     : row == chosen() || row == SPLIT::lower();
    show(units[row], on);
    if (units[row].panel || units[row].door != nullptr) continue;
    light(
      sheet(), icon(units[row].name),
      row == chosen() ? SPLIT::mark(SPLIT::UPPER) : PLAIN);
  }
}

void SOUND::VIEWS::doors() {
  for (const View &unit : registry())
    if (unit.door != nullptr) unit.door();
}

void SOUND::VIEWS::chose() {
  LIT::clear();
  const Vector<View> &units = registry();
  for (Whole row = 0; row < units.size(); ++row)
    if (
      !units[row].panel && units[row].door == nullptr &&
      GUI::GET::clicked(sheet(), icon(units[row].name).c_str()))
      choose(units[row].name);
}

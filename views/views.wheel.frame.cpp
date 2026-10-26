// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

auto lowered() -> const VIEWS::View * {
  const Whole row = VIEWS::SPLIT::lower();
  const Flag own = row < VIEWS::roster().size() && row != VIEWS::chosen();
  return own ? &VIEWS::roster()[row] : nullptr;
}

void turned(const VIEWS::View *unit) {
  if (unit != nullptr && VIEWS::owns(unit->name, VIEWS::ZOOM))
    VIEWS::wheeled(
      *unit,
      GUI::NGA::GET::wheel(VIEWS::sheet(), VIEWS::GET::board(*unit).c_str()));
}

}  // namespace

void SOUND::VIEWS::wheeled() {
  ::turned(found(String(standing())));
  ::turned(::lowered());
  if (!SPLIT::doubled()) return;
  const SPLIT::Turn turn(true);
  ::turned(found(String(standing())));
}

void SOUND::VIEWS::wheeled(const GUI::NGA::Wheel &turn) {
  const View *unit = found(String(standing()));
  if (unit != nullptr) wheeled(*unit, turn);
}

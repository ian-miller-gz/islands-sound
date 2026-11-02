// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

auto west() -> Float {
  return GUI::NGA::GET::pan(
           VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str())
    .x;
}

auto placed(Whole frames) -> Float {
  return (VIEWS::ARRANGEMENT::across(frames) - ::west()) *
         VIEWS::ARRANGEMENT::scaled();
}

auto pulsed(Float place) -> Whole {
  const Float across = place / VIEWS::ARRANGEMENT::scaled() + ::west();
  return VIEWS::pulsed(
    across <= 0.0f ? 0 : Whole(across * Float(VIEWS::ARRANGEMENT::GRAIN)));
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::tape() -> VIEWS::CHANGES::Tape {
  return {strip(), GUTTER, ::placed, ::pulsed};
}

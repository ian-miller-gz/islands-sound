// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

auto west() -> Float {
  return GUI::NGA::GET::pan(VIEWS::document(), VIEWS::ROLL::board().c_str()).x;
}

auto placed(Whole frames) -> Float {
  return (VIEWS::ROLL::across(VIEWS::ROLL::opened(frames)) - ::west()) *
         VIEWS::ROLL::scaled();
}

auto pulsed(Float place) -> Whole {
  const Float across = place / VIEWS::ROLL::scaled() + ::west();
  return across <= 0.0f ? 0 : Whole(across * Float(VIEWS::ROLL::GRAIN));
}

}  // namespace

auto SOUND::VIEWS::ROLL::tape() -> VIEWS::CHANGES::Tape {
  return {strip(), 0.0f, ::placed, ::pulsed};
}

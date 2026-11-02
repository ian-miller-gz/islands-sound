// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;

auto west() -> Float {
  return GUI::NGA::GET::pan(VIEWS::document(), VIEWS::WAVE::board().c_str()).x;
}

auto placed(Whole frames) -> Float {
  return (Float(frames) - ::west()) * VIEWS::WAVE::scaled();
}

auto pulsed(Float place) -> Whole {
  const Float across = place / VIEWS::WAVE::scaled() + ::west();
  return VIEWS::pulsed(across <= 0.0f ? 0 : Whole(across));
}

}  // namespace

auto SOUND::VIEWS::WAVE::tape() -> VIEWS::CHANGES::Tape {
  return {strip(), 0.0f, ::placed, ::pulsed};
}

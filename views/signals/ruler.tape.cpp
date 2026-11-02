// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto west() -> Float {
  return GUI::NGA::GET::pan(VIEWS::document(), VIEWS::SIGNALS::board().c_str())
    .x;
}

auto placed(Whole frames) -> Float {
  return (VIEWS::SIGNALS::across(frames) - ::west()) * VIEWS::SIGNALS::scaled();
}

auto pulsed(Float place) -> Whole {
  const Float across = place / VIEWS::SIGNALS::scaled() + ::west();
  return VIEWS::pulsed(
    across <= 0.0f ? 0 : Whole(across * Float(VIEWS::SIGNALS::GRAIN)));
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::tape() -> VIEWS::CHANGES::Tape {
  return {strip(), 0.0f, ::placed, ::pulsed};
}

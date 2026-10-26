// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include <island/graphics/text/text.hpp>
#include <island/gui/nga.hpp>

#include "piano.pips.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr Float LEAST = 1.0f, STILL = 0.0f;

auto zoomed() -> GUI::NGA::Zoom {
  return GUI::NGA::GET::zoom(VIEWS::document(), ROLL::board().c_str());
}

auto snapped(Float face) -> Float {
  const Float scale = GFX::TEXT::GET::scale();
  return scale <= ::STILL ? face : std::round(face * scale) / scale;
}

auto whole(Float face) -> Float {
  const Float scale = GFX::TEXT::GET::scale();
  return scale <= ::STILL ? face : std::max(::snapped(face), ::LEAST / scale);
}

auto over(Float face, Float zoom) -> Float {
  return zoom <= ::STILL ? face : face / zoom;
}

}  // namespace

auto SOUND::VIEWS::ROLL::WEIGHTS::drawn(Float spaces) -> Float {
  return ::whole(spaces * SPACE);
}

auto SOUND::VIEWS::ROLL::WEIGHTS::drawn(Across weight) -> Float {
  return ::over(drawn(weight.spaces), ::zoomed().value);
}

auto SOUND::VIEWS::ROLL::WEIGHTS::drawn(Down weight) -> Float {
  return ::over(drawn(weight.spaces), ::zoomed().down);
}

auto SOUND::VIEWS::ROLL::WEIGHTS::seated(Float y) -> Float {
  const GUI::Handle page = VIEWS::document();
  const String id = board();
  const Float pan = GUI::NGA::GET::pan(page, id.c_str()).y;
  const Float top = GUI::GET::origin(page, id.c_str()).y;
  const Float down = ::zoomed().down;
  if (down <= ::STILL) return y;
  const Float face = top + (y - pan) * down;
  return pan + (::snapped(face) - top) / down;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "piano.beams.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr Float FULL = ROLL::SPACE * ROLL::STAFF::SCALE;
constexpr Float INKED = ROLL::HALF * ROLL::INK.wide * ROLL::EM;
constexpr Float STILL = 0.0f;

auto reach() -> Float {
  const Float zoom =
    GUI::NGA::GET::zoom(VIEWS::document(), ROLL::board().c_str()).value;
  if (zoom <= ::STILL) return ::INKED;
  return ::INKED * ROLL::HEADS::side(ROLL::plates()) / ::FULL / zoom;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::anchored(const Stem &stem) -> Float {
  const Float middle = across(stem.at) + HALF * SPACE;
  const Float reach = ::reach();
  const Float sides[] = {
    middle + reach - WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::STEM}),
    middle - reach};
  return sides[stem.foot];
}

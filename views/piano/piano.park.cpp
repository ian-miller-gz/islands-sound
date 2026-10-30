// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <utility>

#include <island/gui/nga.hpp>

#include "../views.split.hpp"
#include "piano.internal.hpp"
#include "piano.paper.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;

constexpr Float ORIGIN = 0.0f;

Flag pending = false;

auto below() -> Flag {
  const GUI::Handle page = VIEWS::document();
  return page == VIEWS::SPLIT::second();
}

auto noted() -> Float {
  Float high = ROWS::OFF, low = ROWS::OFF;
  for (const ROLL::Pip &pip : ROLL::pips()) {
    const Float y = ROWS::lane(pip.pitch, pip.at);
    if (pip.stock == NONE || y == ROWS::OFF) continue;
    high = high == ROWS::OFF ? y : std::min(high, y);
    low = std::max(low, y + ROLL::STEP);
  }
  return high == ROWS::OFF ? ROWS::OFF : (high + low) * ROLL::HALF;
}

}  // namespace

void SOUND::VIEWS::ROLL::park() {
  GUI::NGA::set(
    document(), board().c_str(), GUI::NGA::Pan{-margin() / scaled(), ::ORIGIN});
  ::pending = ::below();
}

void SOUND::VIEWS::ROLL::settle() {
  if (!std::exchange(::pending, false)) return;
  const Float middle = ::noted();
  if (middle == ROWS::OFF) return;
  const Float tall = window().h;
  const Float most = std::max(::ORIGIN, ROWS::deep() - tall);
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(document(), board().c_str());
  GUI::NGA::set(
    document(), board().c_str(),
    GUI::NGA::Pan{pan.x, std::clamp(middle - tall * HALF, ::ORIGIN, most)});
}

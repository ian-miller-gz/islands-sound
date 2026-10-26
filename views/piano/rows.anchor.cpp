// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

auto pan() -> GUI::NGA::Pan {
  return GUI::NGA::GET::pan(VIEWS::document(), ROLL::board().c_str());
}

}  // namespace

auto SOUND::VIEWS::ROLL::ROWS::anchored() -> Anchor {
  const GUI::NGA::Pan pan = ::pan();
  const GUI::Extent seen = window();
  const Whole at = across(pan.x + seen.w * HALF);
  const Whole pitch = ROWS::pitch(pan.y + seen.h * HALF, at);
  return {pitch, at, lane(pitch, at) - pan.y};
}

void SOUND::VIEWS::ROLL::ROWS::anchor(const Anchor &held) {
  Whole pitch = held.pitch;
  if (pitch == NONE) return;
  while (pitch > 0 && lane(pitch, held.at) == OFF) --pitch;
  const Float y = lane(pitch, held.at);
  if (y == OFF) return;
  bound();
  GUI::NGA::set(
    document(), board().c_str(), GUI::NGA::Pan{::pan().x, y - held.offset});
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float HALVED = 2.0f;

auto stood(GUI::Handle page, const String &pin) -> GUI::Position {
  const GUI::Position corner =
    GUI::GET::origin(page, VIEWS::WIRED::board().c_str());
  const GUI::Position at = GUI::GET::origin(page, pin.c_str());
  const GUI::Extent size = GUI::GET::measured(page, pin.c_str());
  const GUI::Extent worn = GUI::GET::extent(page, VIEWS::WIRED::RING);
  return {
    at.x + size.w / ::HALVED - corner.x - worn.w / ::HALVED,
    at.y + size.h / ::HALVED - corner.y - worn.h / ::HALVED};
}

}  // namespace

void SOUND::VIEWS::WIRED::aim() {
  const GUI::Handle page = document();
  const String stem = board() + ".";
  const STRING::Cold pin = GUI::NGA::GET::aimed(page);
  const Flag stands =
    !browsing() && !pin.empty() && String(pin).starts_with(stem);
  GUI::set(page, RING, GUI::Visibility{stands});
  if (stands) GUI::set(page, RING, ::stood(page, String(pin)));
}

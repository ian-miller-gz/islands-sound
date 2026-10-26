// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include <island/graphics/windows.hpp>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WHOLE = "root";
constexpr Float NOWHERE = 0.0f;

}  // namespace

auto SOUND::VIEWS::spot() -> GUI::Position {
  const GFX::Viewport rect = GFX::WINDOWS::MAIN::viewport();
  const GUI::Extent page = GUI::GET::measured(document(), ::WHOLE);
  if (rect.w <= ::NOWHERE || rect.h <= ::NOWHERE) return {};
  const INPUT::Pointer &pointer = INPUT::GET::pointer();
  return {
    (pointer.x - rect.x) / rect.w * page.w,
    (pointer.y - rect.y) / rect.h * page.h};
}

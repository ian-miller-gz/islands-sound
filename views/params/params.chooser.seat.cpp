// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "params.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot REGION = "params";
constexpr Float FOOT = 2.0f;

}  // namespace

auto SOUND::VIEWS::RACK::hung(GUI::Handle page, const String &cell)
  -> GUI::Position {
  const GUI::Position at = GUI::GET::origin(page, cell.c_str());
  const GUI::Position corner = GUI::GET::origin(page, ::REGION);
  const Float top = at.y - corner.y;
  const Float tall = GUI::GET::extent(page, PLATE).h;
  const Float below = top + GUI::GET::measured(page, cell.c_str()).h + ::FOOT;
  const Float room = GUI::GET::measured(page, ::REGION).h;
  const Float above = std::max(top - ::FOOT - tall, -corner.y);
  return {at.x - corner.x, below + tall <= room ? below : above};
}

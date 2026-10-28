// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr Float FULL = ROLL::SPACE * ROLL::STAFF::SCALE;
constexpr Float MIDDLE = ROLL::HALF * ROLL::SPACE;
constexpr Float STILL = 0.0f;
constexpr Whole SIXTEENTH = PULSES / 4;

auto zoomed() -> GUI::NGA::Zoom {
  return GUI::NGA::GET::zoom(VIEWS::document(), ROLL::board().c_str());
}

auto gap(const Vector<ROLL::Pip> &pips) -> Whole {
  Vector<Whole> onsets;
  for (const ROLL::Pip &pip : pips) onsets.push_back(pip.at);
  std::ranges::sort(onsets);
  const auto [first, last] = std::ranges::unique(onsets);
  onsets.erase(first, last);
  Whole nearest = NONE;
  for (Whole at = 1; at < onsets.size(); ++at)
    nearest = std::min(nearest, onsets[at] - onsets[at - 1]);
  return nearest;
}

}  // namespace

auto SOUND::VIEWS::ROLL::HEADS::side(const Vector<Pip> &pips) -> Float {
  const Whole nearest = ::gap(pips);
  if (nearest == NONE || nearest >= ::SIXTEENTH) return ::FULL;
  return std::clamp(across(nearest) * ::zoomed().value, LEAST, ::FULL);
}

void SOUND::VIEWS::ROLL::HEADS::sized(
  GUI::Handle page, Whole row, Float side, Float aside) {
  const GUI::NGA::Zoom zoom = ::zoomed();
  if (zoom.value <= ::STILL || zoom.down <= ::STILL) return;
  const Float wide = side / ::FULL / zoom.value;
  const Float tall = side / ::FULL / zoom.down;
  const String id = named(row, HEAD);
  GUI::set(
    page, id.c_str(),
    GUI::Position{
      ::MIDDLE - (::MIDDLE - HEAD.at.x) * wide + aside,
      ::MIDDLE - (::MIDDLE - HEAD.at.y) * tall});
  GUI::set(
    page, id.c_str(), GUI::Extent{HEAD.size.w * wide, HEAD.size.h * tall});
  GUI::set(page, id.c_str(), GUI::Size{HEAD.letter * wide});
  const String sign = named(row, SIGN);
  if (GUI::GET::visibility(page, sign.c_str())) {
    GUI::set(
      page, sign.c_str(),
      GUI::Position{
        ::MIDDLE - (::MIDDLE - SIGN.at.x) * wide + aside,
        ::MIDDLE - (::MIDDLE - SIGN.at.y) * tall});
    GUI::set(
      page, sign.c_str(), GUI::Extent{SIGN.size.w * wide, SIGN.size.h * tall});
    GUI::set(page, sign.c_str(), GUI::Size{SIGN.letter * wide});
  }
  if (GUI::GET::visibility(page, named(row, LEDGER).c_str()))
    ruled(page, row, wide, aside);
}

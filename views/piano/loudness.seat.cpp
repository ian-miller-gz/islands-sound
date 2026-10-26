// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include <island/gui/nga.hpp>

#include "piano.loudness.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace LOUDNESS = SOUND::VIEWS::ROLL::LOUDNESS;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Float ORIGIN = 0.0f;

Float kept = ::ORIGIN;
Whole born = 0;

auto inked() -> Float {
  Float lowest = STAFF::deep();
  for (const STAFF::Stem &stem : STAFF::GET::stems())
    lowest = std::max(lowest, stem.tip);
  return lowest;
}

auto under(GUI::Handle page, const String &board) -> Float {
  const Float tall = GUI::GET::measured(page, board.c_str()).h + ::kept;
  const Float down = GUI::NGA::GET::zoom(page, board.c_str()).down;
  return std::max(LOUDNESS::deep(), std::floor(tall - ::inked() * down));
}

auto wanted(GUI::Handle page, const String &board, Flag open) -> Float {
  if (!open) return ::ORIGIN;
  if (ROLL::GET::notation() != ROLL::PERCUSSION) return LOUDNESS::deep();
  return ::under(page, board);
}

}  // namespace

void SOUND::VIEWS::ROLL::LOUDNESS::inset(GUI::Handle page, Flag open) {
  if (VIEWS::reborn(::born)) ::kept = ::ORIGIN;
  const String board = ROLL::board();
  const Float inset = ::wanted(page, board, open);
  if (inset == ::kept) return;
  ::kept = inset;
  GUI::set(page, board.c_str(), GUI::Extent{::ORIGIN, inset});
}

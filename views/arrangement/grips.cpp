// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "grips.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Carry;

constexpr Float CROWDED = VIEWS::ARRANGEMENT::GRIP * 4.0f;

auto grip() -> Whole {
  return VIEWS::ARRANGEMENT::framed(
    VIEWS::ARRANGEMENT::GRIP / VIEWS::ARRANGEMENT::scaled());
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::gripped(Whole frames) -> Flag {
  return across(frames) * scaled() >= ::CROWDED;
}

auto SOUND::VIEWS::ARRANGEMENT::gutter(Float across) -> Flag {
  const Float pan = GUI::NGA::GET::pan(VIEWS::document(), board().c_str()).x;
  return (across - pan) * scaled() < GUTTER;
}

auto SOUND::VIEWS::ARRANGEMENT::boxed(Float across, Float down) -> Whole {
  const Row seat = row(down);
  if (seat.track == NONE) return NONE;
  if (gutter(across)) return NONE;
  if (shelved(down)) return NONE;
  const Whole at = framed(across);
  const Vector<Box> &laid = boxes();
  for (Whole index = 0; index < laid.size(); ++index)
    if (
      laid[index].track == seat.track && laid[index].lane == seat.lane &&
      at + ::grip() >= laid[index].at &&
      at <= laid[index].at + laid[index].frames + ::grip())
      return index;
  return NONE;
}

auto SOUND::VIEWS::ARRANGEMENT::drawn(Whole row) -> Box {
  const Vector<Box> &laid = boxes();
  if (row >= laid.size()) return {};
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(VIEWS::document());
  if (!gesture.standing) return laid[row];
  const Claim claim = claimed(gesture);
  if (claim.row == NONE) return laid[row];
  if (claim.end != NONE)
    return claim.row == row ? landing(claim, gesture, false) : laid[row];
  for (const Carry &carry : travelled(claim, gesture, false))
    if (carry.row == row) return carry.cut;
  return laid[row];
}

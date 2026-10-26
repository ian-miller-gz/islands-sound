// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/stroke.hpp>

#include "arrangement.internal.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

constexpr STRING::Hot MOVED = "move";
constexpr STRING::Hot SHAPED = "shape";

struct Claim {
  Whole row = NONE;
  Whole end = NONE;
};

struct Carry {
  Whole row = NONE;
  Box cut;
};

struct Travel {
  Flag stood = false;
  Integer by = 0;
};

auto framed(Float across) -> Whole;

void relaid(Vector<Carry> moved);

auto claimed(const GUI::SAC::STROKE::Gesture &gesture) -> Claim;

auto landing(
  const Claim &claim, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Box;

auto travelled(
  const Claim &claim, const GUI::SAC::STROKE::Gesture &gesture,
  Flag settle) -> Vector<Carry>;

auto roomed(
  const Vector<Whole> &group, const Box &lead, Integer asked,
  Integer hand) -> Travel;

auto walled(const Claim &claim, Whole landed) -> Whole;

}  // namespace SOUND::VIEWS::ARRANGEMENT

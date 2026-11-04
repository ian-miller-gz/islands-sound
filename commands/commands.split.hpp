// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../history.hpp"
#include "../timeline.hpp"

namespace SOUND::COMMANDS::SPLIT {

struct Yield {
  Vector<Placement> lifted;
  Vector<Placement> kept;
};

auto cut(const ARRANGEMENT::TRACK::LANE::Clip &span, Whole kind, Whole at)
  -> Vector<ARRANGEMENT::TRACK::LANE::Clip>;

auto yielded(
  const Vector<Placement> &landing, const Vector<Placement> &ignoring)
  -> Yield;

}  // namespace SOUND::COMMANDS::SPLIT

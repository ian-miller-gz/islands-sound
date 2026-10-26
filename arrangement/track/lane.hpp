// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../kind.hpp"
#include "../../score.hpp"

namespace SOUND::ARRANGEMENT::TRACK {

namespace LANE {

struct Clip {
  Whole stock = NONE;
  Whole at = 0;
  Whole from = 0;
  Whole frames = 0;
};

}  // namespace LANE

struct Lane {
  Whole kind = KIND::NOTES;
  String name;
  String map;
  Vector<LANE::Clip> clips;
  Vector<LANE::Clip> outtakes;
};

}  // namespace SOUND::ARRANGEMENT::TRACK

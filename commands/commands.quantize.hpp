// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../timeline.hpp"

namespace SOUND::COMMANDS::QUANTIZE {

auto cut(const ARRANGEMENT::TRACK::LANE::Clip &clip, Whole grid, Float sense)
  -> Vector<ARRANGEMENT::TRACK::LANE::Clip>;

}  // namespace SOUND::COMMANDS::QUANTIZE

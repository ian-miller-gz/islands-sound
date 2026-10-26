// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../timeline.hpp"

namespace SOUND::COMMANDS::SPLIT {

auto cut(const ARRANGEMENT::TRACK::LANE::Clip &span, Whole kind, Whole at)
  -> Vector<ARRANGEMENT::TRACK::LANE::Clip>;

}  // namespace SOUND::COMMANDS::SPLIT

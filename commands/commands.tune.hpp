// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../timeline.hpp"

namespace SOUND::COMMANDS::TUNE {

auto tunes(const ARRANGEMENT::TRACK::LANE::Clip &plain, Float strength)
  -> Vector<ARRANGEMENT::TRACK::LANE::Tune>;

}  // namespace SOUND::COMMANDS::TUNE

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../timeline.hpp"

namespace SOUND::COMMANDS {

auto tailed(const ARRANGEMENT::TRACK::LANE::Clip &span) -> String;

auto retouch(
  Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &now, STRING::Hot act) -> Flag;

}  // namespace SOUND::COMMANDS

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../timeline.hpp"
#include "views.tune.hpp"

auto SOUND::VIEWS::tuned(
  Whole track, Whole lane, Whole placement, Float strength) -> Flag {
  if (strength <= TIMELINE::TUNE::LEAST)
    return COMMANDS::untuned(track, lane, placement);
  return COMMANDS::tuned(track, lane, placement, strength) != NONE;
}

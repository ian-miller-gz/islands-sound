// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

auto SOUND::TIMELINE::wrapped(Whole from, Whole elapsed, Whole carried)
  -> Whole {
  return carried == 0 ? 0 : (from + elapsed) % carried;
}

auto SOUND::TIMELINE::opened(Whole at, Whole from, Whole carried) -> Whole {
  if (carried == 0) return 0;
  if (at >= from) return at - from;
  return (at + carried - from % carried) % carried;
}

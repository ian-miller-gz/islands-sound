// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "../views.attention.hpp"
#include "trace.internal.hpp"

auto SOUND::VIEWS::TRACE::attended() -> Whole {
  const Whole track = ATTENTION::attended(ATTENTION::current());
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

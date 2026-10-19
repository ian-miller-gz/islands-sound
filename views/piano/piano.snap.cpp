// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

auto SOUND::VIEWS::ROLL::degreed(Whole pitch, Whole at) -> Whole {
  if (GET::notation() == PERCUSSION) return pitch;
  const Whole landing = SCORE::snapped(TRANSPORT::keyed(at), pitch);
  return landing < HIGHEST ? landing : HIGHEST;
}

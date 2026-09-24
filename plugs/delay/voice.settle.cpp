// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float WHOLE = 1.0f;
constexpr Float SECOND = 1000.0f;
constexpr Whole SHORTEST = 1;

auto spanned(Whole rate, Float milliseconds) -> Whole {
  const Float frames = Float(rate) * milliseconds / SECOND;
  return frames <= Float(SHORTEST) ? SHORTEST : Whole(frames);
}

}  // namespace

void SOUND::DELAY::stretch(Trail &trail) {
  trail.slots = ::spanned(trail.rate, LONGEST) + SHORTEST;
  trail.lanes.assign(trail.channels, Lane{});
  for (auto &lane : trail.lanes) lane.ring.assign(trail.slots, 0);
}

void SOUND::DELAY::settle(Trail &trail) {
  const Whole reach = trail.slots - SHORTEST;
  const Whole asked = ::spanned(trail.rate, trail.rows[TIME]);
  trail.spacing = asked > reach ? reach : asked;
  trail.given = trail.rows[FEEDBACK] / WHOLLY;
  trail.wet = trail.rows[MIX] / WHOLLY;
  trail.dry = WHOLE - trail.wet;
}

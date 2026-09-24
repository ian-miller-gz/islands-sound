// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float TURN = 6.2831853f;
constexpr Float PAIR = 2.0f;
constexpr Float WHOLE = 1.0f;

constexpr Float FLAT = 1.4142136f;
constexpr Float RINGING = 0.1f;

auto tangent(Float hertz, Whole rate) -> Float {
  const Float top = Float(rate) / Float(FILTER::CLEAR);
  const Float held = hertz > top ? top : hertz;
  return std::tan(TURN * held / (PAIR * Float(rate)));
}

auto corner(const FILTER::Sieve &sieve, Float hertz) -> Float {
  const Float place = (hertz - FILTER::LOWEST) / FILTER::STEP;
  const Whole whole = Whole(place < 0 ? 0 : place);
  if (whole >= FILTER::CORNERS) return sieve.corners[FILTER::CORNERS];
  const Float part = place - Float(whole);
  const Float step = sieve.corners[whole + 1] - sieve.corners[whole];
  return sieve.corners[whole] + step * part;
}

}  // namespace

void SOUND::FILTER::bake(Sieve &sieve) {
  sieve.corners.assign(CORNERS + 1, 0);
  for (Whole place = 0; place <= CORNERS; ++place)
    sieve.corners[place] = ::tangent(LOWEST + STEP * Float(place), sieve.rate);
}

void SOUND::FILTER::settle(Sieve &sieve) {
  sieve.mode = Whole(sieve.rows[MODE]);
  sieve.damping = FLAT - sieve.rows[RESONANCE] * (FLAT - RINGING);
  const Float held = ::corner(sieve, sieve.rows[CUTOFF]);
  sieve.loop = WHOLE / (WHOLE + held * (held + sieve.damping));
  sieve.feed = held * sieve.loop;
  sieve.carry = held * sieve.feed;
}

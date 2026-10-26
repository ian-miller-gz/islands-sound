// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "timeline.shift.hpp"

namespace SHIFT = SOUND::TIMELINE::SHIFT;

auto SHIFT::held(Integer cents) -> Integer {
  return std::clamp(cents, -SPAN * SEMITONE, SPAN * SEMITONE);
}

auto SHIFT::split(Integer cents) -> Interval {
  const Integer semitones =
    Integer(std::lround(Float(cents) / Float(SEMITONE)));
  return {.semitones = semitones, .cents = cents - semitones * SEMITONE};
}

auto SHIFT::joined(const Interval &interval) -> Integer {
  return held(interval.semitones * SEMITONE + interval.cents);
}

auto SHIFT::shifted(const Clip &clip, Integer cents) -> Clip {
  Clip now = clip;
  now.cents = held(cents);
  return now;
}

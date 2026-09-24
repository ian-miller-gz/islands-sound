// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "score.doors.hpp"

namespace {

constexpr Float MINUTE = 60.0f;

auto sooner(const SOUND::Note &one, const SOUND::Note &other) -> Flag {
  return one.at < other.at;
}

}  // namespace

auto SOUND::SCORE::framed(Whole pulses, Float tempo) -> Whole {
  if (tempo <= 0) return 0;
  const Float quarters = static_cast<Float>(pulses) / PULSES;
  return static_cast<Whole>(quarters * ::MINUTE / tempo * RATE);
}

auto SOUND::SCORE::pulsed(Whole frames, Float tempo) -> Whole {
  if (tempo <= 0) return 0;
  const Float quarters = static_cast<Float>(frames) / RATE * tempo / ::MINUTE;
  return static_cast<Whole>(quarters * PULSES + 0.5f);
}

auto SOUND::SCORE::write(Score &score, const Note &note) -> Whole {
  const auto after =
    std::upper_bound(score.notes.begin(), score.notes.end(), note, ::sooner);
  const auto landed = static_cast<Whole>(after - score.notes.begin());
  score.notes.insert(after, note);
  return landed;
}

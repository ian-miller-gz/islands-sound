// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <limits>

#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Float ORIGIN = 0.0f;
constexpr Integer FAR = std::numeric_limits<Integer>::max();

auto row(Float y) -> Integer {
  return y <= ::ORIGIN ? 0 : Integer(y / ROLL::STEP + ROLL::HALF);
}

auto placed(Integer place) -> Whole {
  for (const STAFF::Line &line : STAFF::GET::lines())
    if (line.place == place) return line.note;
  return NONE;
}

auto nearest(Integer place) -> Whole {
  Whole note = NONE;
  Integer best = ::FAR;
  for (const STAFF::Line &line : STAFF::GET::lines())
    if (std::abs(line.place - place) < best) {
      best = std::abs(line.place - place);
      note = line.note;
    }
  return note;
}

constexpr auto (*PRESSED[])(Integer) -> Whole = {::nearest, ::placed};

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::lane(Whole pitch, Whole) -> Float {
  const Integer place = placed(pitch);
  return place == NOWHERE ? ROWS::OFF : Float(place - TOP) * STEP;
}

auto SOUND::VIEWS::ROLL::STAFF::pitch(Float y, Whole) -> Whole {
  return ::PRESSED[ROLL::GET::presentation()](TOP + ::row(y));
}

auto SOUND::VIEWS::ROLL::STAFF::deep() -> Float {
  Integer lowest = FOOT;
  for (const Line &line : GET::lines()) lowest = std::max(lowest, line.place);
  return Float(lowest - TOP + 1) * STEP;
}

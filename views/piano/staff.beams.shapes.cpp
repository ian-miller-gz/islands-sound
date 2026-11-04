// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "piano.beams.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr Float FLAG = ROLL::SPACE;

auto upright() -> Float {
  return WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::STEM});
}

auto along(const STAFF::Ends &line, Float x) -> GUI::Position {
  const Float share = (x - line.near.x) / (line.far.x - line.near.x);
  return {x, line.near.y + share * (line.far.y - line.near.y)};
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::centred(Integer place) -> Float {
  return (Float(place - top()) + HALF) * STEP;
}

auto SOUND::VIEWS::ROLL::STAFF::tipped(Integer place, Flag foot) -> Float {
  return centred(place) + WAYS[foot] * LENGTH * SPACE;
}

auto SOUND::VIEWS::ROLL::STAFF::boxed(const Stem &stem) -> Box {
  const Float head = centred(stem.from);
  const Float top = std::min(head, stem.tip);
  return {{anchored(stem), top}, {::upright(), std::abs(stem.tip - head)}};
}

auto SOUND::VIEWS::ROLL::STAFF::ended(
  const Beam &beam, const Vector<Stem> &stems) -> Ends {
  const Stem &stem = stems[beam.stem];
  const Stem &far = beam.to == NONE ? stem : stems[beam.to];
  const Float deep = WEIGHTS::drawn(WEIGHTS::Down{WEIGHTS::BEAM});
  const Float rank = deep + WEIGHTS::drawn(WEIGHTS::Down{WEIGHTS::GAP});
  const Float lean = -WAYS[stem.foot] * (HALF * deep + Float(beam.rank) * rank);
  const Float inset = HALF * ::upright();
  const Float west = anchored(stem);
  const Float east =
    beam.to == NONE ? west + ::FLAG : anchored(far) + ::upright();
  const Ends line = {{west, stem.tip + lean}, {east, far.tip + lean}};
  return {::along(line, west + inset), ::along(line, east - inset)};
}

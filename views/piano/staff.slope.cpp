// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <numeric>
#include <utility>

#include "piano.beams.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Integer QUARTERS = 4;
constexpr Float QUARTER = ROLL::SPACE / Float(QUARTERS);
constexpr Float STANDARD = STAFF::LENGTH * ROLL::SPACE;
constexpr Float REFUSED = HUGE_VALF;
constexpr Float PENALTIES[] = {4.0f, 1.0f};

struct Group {
  Whole first = NONE, last = NONE;
  Float way = 0.0f, ideal = 0.0f, middle = 0.0f, edge = -HUGE_VALF;
  Vector<Float> heads, shares;
};

Vector<STAFF::Slope> found;

auto head(const STAFF::Stem &stem) -> Float {
  return stem.tip - STAFF::WAYS[stem.foot] * ::STANDARD;
}

auto cost(const Group &group, Float rise) -> Float {
  Float extension = 0.0f;
  for (Whole at = 0; at < group.heads.size(); ++at) {
    const Float tip = group.middle + rise * group.shares[at];
    const Float length = group.way * (tip - group.heads[at]);
    if (length < STAFF::SHORTEST * ROLL::SPACE) return ::REFUSED;
    extension += std::abs(length - ::STANDARD);
  }
  const Float terms[] = {std::abs(rise - group.ideal), extension};
  const Float total = std::inner_product(
    std::begin(terms), std::end(terms), std::begin(::PENALTIES), 0.0f);
  return total / ROLL::SPACE;
}

auto chosen(Group &group) -> Float {
  const Float cap = group.ideal == 0.0f ? 0.0f : STAFF::CAP * ROLL::SPACE;
  Float rise = 0.0f;
  for (Float tried = -cap; tried <= cap; tried += ::QUARTER)
    if (::cost(group, tried) < ::cost(group, rise)) rise = tried;
  if (::cost(group, rise) < ::REFUSED) return rise;
  group.middle = group.way * (group.edge + ::STANDARD);
  return 0.0f;
}

void sloped(Vector<STAFF::Stem> &stems, Group group) {
  if (group.first == NONE) return;
  const STAFF::Stem &west = stems[group.first], &east = stems[group.last];
  group.way = STAFF::WAYS[west.foot];
  group.ideal = ROLL::HALF * (::head(east) - ::head(west));
  group.middle = ROLL::HALF * (west.tip + east.tip);
  for (Whole at = group.first; at <= group.last; ++at) {
    group.heads.push_back(::head(stems[at]));
    group.shares.push_back(
      Float(stems[at].at - west.at) / Float(east.at - west.at) - ROLL::HALF);
    group.edge = std::fmax(group.edge, group.way * group.heads.back());
  }
  const Float rise = ::chosen(group);
  for (Whole at = group.first; at <= group.last; ++at)
    stems[at].tip = group.middle + rise * group.shares[at - group.first];
  ::found.push_back({west.at, Integer(std::lround(-rise / ::QUARTER))});
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::GET::slopes() -> const Vector<Slope> & {
  return ::found;
}

auto SOUND::VIEWS::ROLL::STAFF::sloped(
  Vector<Stem> &stems, const Vector<Beam> &beams) -> void {
  ::found.clear();
  Group group;
  for (const Beam &beam : beams) {
    if (beam.rank != 0 || beam.to == NONE) continue;
    if (group.last != beam.stem) ::sloped(stems, std::exchange(group, {}));
    if (group.first == NONE) group.first = beam.stem;
    group.last = beam.to;
  }
  ::sloped(stems, group);
}

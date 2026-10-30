// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>
#include <tuple>

#include "piano.beams.hpp"
#include "piano.heads.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

struct Hit {
  Whole at = 0;
  Integer place = 0;
  Flag foot = false;
  auto operator==(const Hit &) const -> Bool = default;
};

constexpr Integer MIDDLE = 4;

Vector<Hit> kept;
Vector<STAFF::Stem> stood;
Whole way = NONE;

auto before(const Hit &one, const Hit &two) -> Flag {
  return std::tie(one.foot, one.at, one.place) <
         std::tie(two.foot, two.at, two.place);
}

auto hits() -> Vector<Hit> {
  Vector<Hit> found;
  if (!ROLL::GET::staffed()) return found;
  const Flag pitched = ROLL::GET::pitched();
  for (const ROLL::Pip &pip : ROLL::plates()) {
    const Integer place = STAFF::placed(pip.pitch);
    const ROLL::Drum *drum = ROLL::GET::drum(pip.pitch);
    if (place == STAFF::NOWHERE) continue;
    const Flag foot = pitched ? place < ::MIDDLE : drum != nullptr && drum->foot;
    found.push_back({pip.at, place, foot});
  }
  std::ranges::sort(found, ::before);
  return found;
}

auto grouped(const Vector<Hit> &hits) -> Vector<STAFF::Stem> {
  Vector<STAFF::Stem> stems;
  for (const Hit &hit : hits) {
    const Flag shared = !stems.empty() && stems.back().at == hit.at &&
                        stems.back().foot == hit.foot;
    if (!shared)
      stems.push_back(
        {hit.at, hit.place, STAFF::tipped(hit.place, hit.foot), 0, hit.foot});
    else if (hit.foot)
      stems.back().tip = STAFF::tipped(hit.place, hit.foot);
    else
      stems.back().from = hit.place;
  }
  return stems;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::GET::stems() -> const Vector<Stem> & {
  return ::stood;
}

void SOUND::VIEWS::ROLL::STAFF::stemmed() {
  Vector<Hit> hits = ::hits();
  const Whole way = ROLL::GET::presentation();
  const Flag moved = hits != ::kept || way != ::way;
  if (moved) {
    ::way = way;
    ::kept = std::move(hits);
    ::stood = ::grouped(::kept);
    valued(::stood);
    sloped(::stood, joined(::stood));
  }
  beamed(moved);
}

void SOUND::VIEWS::ROLL::STAFF::SHED::stemmed() {
  ::way = NONE;
  ::kept.clear();
  ::stood.clear();
  sloped(::stood, {});
  beamed();
}

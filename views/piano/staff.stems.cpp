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

Vector<Hit> kept;
Vector<STAFF::Stem> stood;

auto before(const Hit &one, const Hit &two) -> Flag {
  return std::tie(one.foot, one.at, one.place) <
         std::tie(two.foot, two.at, two.place);
}

auto hits() -> Vector<Hit> {
  Vector<Hit> found;
  if (ROLL::GET::notation() != ROLL::PERCUSSION) return found;
  for (const ROLL::Pip &pip : ROLL::plates()) {
    const Integer place = STAFF::placed(pip.pitch);
    const ROLL::Drum *drum = ROLL::GET::drum(pip.pitch);
    if (place != STAFF::NOWHERE)
      found.push_back({pip.at, place, drum != nullptr && drum->foot});
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

auto valued(Whole gap) -> Whole {
  for (const STAFF::Value &value : STAFF::VALUES)
    if (gap * value.part >= PULSES) return value.beams;
  return std::rbegin(STAFF::VALUES)->beams;
}

void valued(Vector<STAFF::Stem> &stems) {
  for (Whole stem = 0; stem < stems.size(); ++stem) {
    Whole close = STAFF::beaten(stems[stem].at) + PULSES;
    const Whole next = stem + 1;
    if (next < stems.size() && stems[next].foot == stems[stem].foot)
      close = std::min(close, stems[next].at);
    stems[stem].beams = ::valued(close - stems[stem].at);
  }
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::GET::stems() -> const Vector<Stem> & {
  return ::stood;
}

void SOUND::VIEWS::ROLL::STAFF::stemmed() {
  Vector<Hit> hits = ::hits();
  const Flag moved = hits != ::kept;
  if (moved) {
    ::kept = std::move(hits);
    ::stood = ::grouped(::kept);
    ::valued(::stood);
    sloped(::stood, joined(::stood));
  }
  beamed(moved);
}

void SOUND::VIEWS::ROLL::STAFF::SHED::stemmed() {
  ::kept.clear();
  ::stood.clear();
  sloped(::stood, {});
  beamed();
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "piano.internal.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

auto paired(const STAFF::Stem &one, const STAFF::Stem &two) -> Flag {
  return one.foot == two.foot && STAFF::beaten(one.at) == STAFF::beaten(two.at);
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::beaten(Whole pulse) -> Whole {
  const TRANSPORT::Section section = VIEWS::standing(ROLL::sections(), pulse);
  const Whole into = pulse > section.at ? pulse - section.at : 0;
  return pulse - into % PULSES;
}

auto SOUND::VIEWS::ROLL::STAFF::joined(const Vector<Stem> &stems)
  -> Vector<Beam> {
  Vector<Beam> found;
  for (Whole at = 0; at < stems.size(); ++at) {
    const Whole next = at + 1;
    const Flag west = at > 0 && ::paired(stems[at - 1], stems[at]);
    const Flag east = next < stems.size() && ::paired(stems[at], stems[next]);
    const Whole lone = west ? 0 : stems[at].beams;
    const Whole ranks =
      east ? std::min(stems[at].beams, stems[next].beams) : lone;
    for (Whole rank = 0; rank < ranks; ++rank)
      found.push_back({at, rank, east ? next : NONE});
  }
  return found;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "piano.beams.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

auto plain() -> Flag {
  return ROLL::GET::presentation() == ROLL::PRESENTATION::GRID;
}

auto valued(Whole gap) -> Whole {
  for (const STAFF::Value &value : STAFF::VALUES)
    if (gap * value.part >= PULSES) return value.beams;
  return std::rbegin(STAFF::VALUES)->beams;
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::valued(Vector<Stem> &stems) {
  if (::plain()) return;
  for (Whole stem = 0; stem < stems.size(); ++stem) {
    Whole close = beaten(stems[stem].at) + PULSES;
    const Whole next = stem + 1;
    if (next < stems.size() && stems[next].foot == stems[stem].foot)
      close = std::min(close, stems[next].at);
    stems[stem].beams = ::valued(close - stems[stem].at);
  }
}

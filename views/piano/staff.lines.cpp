// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

Vector<STAFF::Line> kept;

auto pitches() -> Vector<Whole> {
  Vector<Whole> found;
  for (const ROLL::Pip &pip : ROLL::pips()) found.push_back(pip.pitch);
  std::ranges::sort(found);
  found.erase(std::ranges::unique(found).begin(), found.end());
  return found;
}

auto composed() -> Vector<STAFF::Line> {
  if (ROLL::GET::notation() != ROLL::PERCUSSION) return {};
  const Vector<STAFF::Line> lines =
    STAFF::placed(ROLL::GET::voices(), ::pitches());
  if (ROLL::GET::presentation() != ROLL::PRESENTATION::GRID) return lines;
  return STAFF::gridded(lines);
}

auto same(const STAFF::Line &one, const STAFF::Line &two) -> Flag {
  return one.place == two.place && one.note == two.note && one.name == two.name;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::GET::lines() -> const Vector<Line> & {
  return ::kept;
}

auto SOUND::VIEWS::ROLL::STAFF::placed(Whole pitch) -> Integer {
  for (const Line &line : ::kept)
    if (line.note == pitch) return line.place;
  return NOWHERE;
}

auto SOUND::VIEWS::ROLL::STAFF::relined() -> Flag {
  Vector<Line> lines = ::composed();
  if (std::ranges::equal(lines, ::kept, ::same)) return false;
  ::kept = std::move(lines);
  return true;
}

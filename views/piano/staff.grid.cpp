// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Integer GAP = STAFF::LINES[1] - STAFF::LINES[0];
constexpr Whole COUNT = std::size(ROLL::PRESENTATIONS);

auto places(const Vector<STAFF::Line> &lines) -> Vector<Integer> {
  Vector<Integer> found;
  for (const STAFF::Line &line : lines) found.push_back(line.place);
  std::ranges::sort(found);
  found.erase(std::ranges::unique(found).begin(), found.end());
  return found;
}

auto staffed() -> Vector<Integer> {
  return {std::begin(STAFF::LINES), std::end(STAFF::LINES)};
}

auto gridded() -> Vector<Integer> { return ::places(STAFF::GET::lines()); }

constexpr auto (*RULES[])() -> Vector<Integer> = {::gridded, ::staffed};

}  // namespace

auto SOUND::VIEWS::ROLL::GET::presentation() -> Whole {
  const Whole presentation = faced().presentation;
  return presentation < ::COUNT ? presentation : PRESENTATION::GRID;
}

auto SOUND::VIEWS::ROLL::STAFF::GET::rules() -> Vector<Integer> {
  return ::RULES[ROLL::GET::presentation()]();
}

auto SOUND::VIEWS::ROLL::STAFF::gridded(const Vector<Line> &lines)
  -> Vector<Line> {
  const Vector<Integer> ranks = ::places(lines);
  Vector<Line> grid = lines;
  for (Line &line : grid)
    line.place =
      ::GAP *
      Integer(std::ranges::lower_bound(ranks, line.place) - ranks.begin());
  return grid;
}

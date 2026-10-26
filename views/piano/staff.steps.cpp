// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <ranges>

#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;
using STAFF::Line;

auto lower(const Vector<Line> &lines, Integer wanted) -> Integer {
  const auto found = std::ranges::find_if(
    lines, [wanted](const Line &line) { return line.place >= wanted; });
  return found == lines.end() ? lines.back().place : found->place;
}

auto upper(const Vector<Line> &lines, Integer wanted) -> Integer {
  const auto found = std::ranges::find_if(
    lines | std::views::reverse,
    [wanted](const Line &line) { return line.place <= wanted; });
  return found == lines.rend() ? lines.front().place : found->place;
}

auto first(const Vector<Line> &lines, Integer place) -> Whole {
  return std::ranges::find(lines, place, &Line::place)->note;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::stepped(Whole pitch, Integer by) -> Whole {
  const Vector<Line> &lines = GET::lines();
  const Integer from = placed(pitch);
  if (by == 0 || from == NOWHERE) return pitch;
  const Integer wanted = from + by;
  const Integer place =
    by > 0 ? ::lower(lines, wanted) : ::upper(lines, wanted);
  return ::first(lines, place);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../commands.hpp"
#include "views.ends.hpp"

namespace {
using namespace SOUND;
using VIEWS::ENDS::Bed;
using VIEWS::ENDS::Line;
using VIEWS::ENDS::Pool;

constexpr Float NORTH = 0.0f;
constexpr Float UNWORN = -1.0f;

auto placed(const Bed &bed, const Line &line) -> Float {
  const Float at = Float(line.length - line.floor) / Float(bed.grain);
  return std::clamp(at, 0.0f, line.room);
}

void written(GUI::Handle page, const Bed &bed, const Line &line, Whole row) {
  const String head = VIEWS::ENDS::named(bed, row) + "." + VIEWS::ENDS::HEAD;
  const Float at = std::clamp(
    GUI::GET::position(page, head.c_str()).x, 0.0f, line.room);
  const Whole length =
    line.floor + Whole(std::lround(at * Float(bed.grain)));
  COMMANDS::lengthened(line.stock, length);
}

void laid(
  GUI::Handle page, const Bed &bed, const Line &line, Whole row, Pool &pool) {
  const String id = VIEWS::ENDS::named(bed, row);
  const String head = id + "." + VIEWS::ENDS::HEAD;
  const Float wide = VIEWS::ENDS::headed(bed);
  GUI::set(page, id.c_str(), GUI::Position{line.west, ::NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{line.room + wide, bed.deep});
  const Float at = ::placed(bed, line);
  if (pool.worn[row] != at) {
    GUI::set(page, head.c_str(), GUI::Position{at, ::NORTH});
    GUI::set(page, head.c_str(), GUI::Extent{wide, wide});
    pool.worn[row] = at;
  }
  const String run = id + "." + VIEWS::ENDS::RUN;
  const Float stands = GUI::GET::position(page, head.c_str()).x;
  GUI::set(page, run.c_str(), GUI::Position{stands, ::NORTH});
  GUI::set(page, run.c_str(), GUI::Ends{{::NORTH, bed.deep}});
}

}  // namespace

void SOUND::VIEWS::ENDS::lay(
  GUI::Handle page, const Bed &bed, const Vector<Line> &lines, Pool &pool) {
  if (lines.size() != pool.stood) build(page, bed, lines.size(), pool);
  for (Whole row = 0; row < lines.size(); ++row) {
    const String head = named(bed, row) + "." + HEAD;
    if (GUI::GET::moved(page, head.c_str())) {
      ::written(page, bed, lines[row], row);
      pool.worn[row] = ::UNWORN;
      continue;
    }
    ::laid(page, bed, lines[row], row, pool);
  }
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto column(const VIEWS::SIGNALS::Point &point, Float wide) -> Float {
  return std::floor(VIEWS::SIGNALS::across(point.at) / wide);
}

auto shared(
  const VIEWS::SIGNALS::Point &row, const VIEWS::SIGNALS::Point &next,
  Float wide) -> Flag {
  return row.index != NONE && next.index != NONE && row.stock == next.stock &&
         ::column(row, wide) == ::column(next, wide);
}

void joined(VIEWS::SIGNALS::Point &row, const VIEWS::SIGNALS::Point &next) {
  const Flag folding = row.high > row.low;
  const Float low = folding ? row.low : row.value;
  const Float high = folding ? row.high : row.value;
  row = next;
  row.low = std::min(low, next.value);
  row.high = std::max(high, next.value);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::folded(const Vector<Point> &points, Float scale)
  -> Vector<Point> {
  const Float wide = scale > 0.0f ? PLATE / scale : PLATE;
  Vector<Point> rows;
  for (const Point &point : points)
    if (!rows.empty() && ::shared(rows.back(), point, wide))
      ::joined(rows.back(), point);
    else
      rows.push_back(point);
  return rows;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto placed(Whole frames) -> Float { return VIEWS::SIGNALS::across(frames); }

auto folded(Float value) -> Float {
  return (Float(VIEWS::SIGNALS::FULL) - value) /
         Float(VIEWS::SIGNALS::FULL + 1);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::overview() -> VIEWS::Overview {
  Overview read;
  read.placed = ::placed;
  read.reach = bar();
  for (const Point &row : shown()) {
    read.reach = std::max(read.reach, row.until);
    read.dashes.push_back(
      {across(row.at), across(row.until - row.at), ::folded(row.value)});
  }
  return read;
}

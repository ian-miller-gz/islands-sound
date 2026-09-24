// SPDX-License-Identifier: AGPL-3.0-or-later
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto corridor() -> Float {
  return VIEWS::SIGNALS::across(VIEWS::SIGNALS::grain()) / VIEWS::SIGNALS::STEP;
}

auto standing(
  const VIEWS::SIGNALS::Point &one, const VIEWS::SIGNALS::Point &two,
  Whole at) -> Float {
  if (two.at <= one.at) return one.value;
  return one.value +
         (two.value - one.value) * Float(at - one.at) / Float(two.at - one.at);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::thinned(Vector<Point> &drawn) {
  if (drawn.size() < 3) return;
  const Float corridor = ::corridor();
  Vector<Point> kept;
  kept.push_back(drawn.front());
  for (Whole row = 1; row + 1 < drawn.size(); ++row) {
    const Point &node = drawn[row];
    const Float line = ::standing(kept.back(), drawn[row + 1], node.at);
    const Float off = node.value > line ? node.value - line : line - node.value;
    if (node.shape != SHAPE::LINE || off > corridor) kept.push_back(node);
  }
  kept.push_back(drawn.back());
  drawn = kept;
}

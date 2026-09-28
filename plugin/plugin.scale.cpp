// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "plugin.hpp"

namespace {

auto landed(const AUDIO::PLUGIN::Control &described, Float fraction) -> Float {
  const Float steps = Float(described.steps);
  const Float place = std::round(fraction * steps);
  return described.least + (described.most - described.least) * place / steps;
}

}  // namespace

auto SOUND::PLUGIN::scaled(const AUDIO::PLUGIN::Control &described, Float drawn)
  -> Float {
  if (described.most <= described.least) return drawn;
  const Float fraction = std::clamp(drawn, Float(0), FULL) / FULL;
  if (described.steps == 0)
    return described.least + (described.most - described.least) * fraction;
  return ::landed(described, fraction);
}

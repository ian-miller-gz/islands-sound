// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "rig.hpp"

namespace {
using namespace SOUND;

auto worded(
  const AUDIO::PLUGIN::Control &published, const String &run,
  Float &value) -> Flag {
  if (published.steps == 0 || published.labels.size() != published.steps + 1)
    return false;
  for (Whole step = 0; step < published.labels.size(); ++step) {
    if (published.labels[step] != run) continue;
    value = VIEWS::RIG::valued(published, step);
    return true;
  }
  return false;
}

auto numbered(const String &run, Float &value) -> Flag {
  char *end = nullptr;
  value = std::strtof(run.c_str(), &end);
  return end != run.c_str() && std::isfinite(value);
}

auto settled(const AUDIO::PLUGIN::Control &published, Float value) -> Float {
  if (published.most <= published.least) return value;
  return std::clamp(value, published.least, published.most);
}

}  // namespace

auto SOUND::VIEWS::RIG::parsed(
  const AUDIO::PLUGIN::Control &published, const String &run,
  Float &value) -> Flag {
  if (!::worded(published, run, value) && !::numbered(run, value)) return false;
  value = ::settled(published, value);
  return true;
}

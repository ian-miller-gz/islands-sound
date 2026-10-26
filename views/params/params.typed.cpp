// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "params.internal.hpp"

namespace {
using namespace SOUND;

auto worded(
  const AUDIO::PLUGIN::Control &published, const String &run,
  Float &value) -> Flag {
  if (published.steps == 0 || published.labels.size() != published.steps + 1)
    return false;
  for (Whole step = 0; step < published.labels.size(); ++step) {
    if (published.labels[step] != run) continue;
    value = VIEWS::RACK::valued(published, step);
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

void SOUND::VIEWS::RACK::typed(
  GUI::Handle page, const String &cell, const Row &row) {
  const String barred = cell + "." + BARRED;
  if (!GUI::GET::committed(page, barred.c_str())) return;
  GUI::edit(page, "");
  const AUDIO::PLUGIN::Control published = described(row);
  const String run = GUI::GET::text(page, barred.c_str());
  Float value = 0.0f;
  if (::worded(published, run, value) || ::numbered(run, value))
    moved(row, ::settled(published, value));
}

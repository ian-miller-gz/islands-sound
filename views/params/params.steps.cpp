// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "params.internal.hpp"

namespace {
constexpr STRING::Hot DOOR = "steps";
}  // namespace

auto SOUND::VIEWS::RACK::stepped(
  const AUDIO::PLUGIN::Control &published, Float value) -> Whole {
  const Float span = published.most - published.least;
  const Float fraction = std::clamp(
    span > 0.0f ? (value - published.least) / span : value, 0.0f, 1.0f);
  return Whole(fraction * Float(published.steps) + 0.5f);
}

auto SOUND::VIEWS::RACK::valued(
  const AUDIO::PLUGIN::Control &published, Whole step) -> Float {
  const Float span = published.most - published.least;
  return published.least + span * Float(step) / Float(published.steps);
}

auto SOUND::VIEWS::RACK::listed(const Row &row) -> Flag {
  const AUDIO::PLUGIN::Control published = described(row);
  return published.steps > 1 && published.labels.size() == published.steps + 1;
}

void SOUND::VIEWS::RACK::door(
  GUI::Handle page, const String &cell, const Row &row) {
  const String id = cell + "." + ::DOOR;
  GUI::set(page, id.c_str(), GUI::Visibility{listed(row)});
  if (GUI::GET::clicked(page, id.c_str())) return raise(page, id, row);
  seated(page, id, row);
}

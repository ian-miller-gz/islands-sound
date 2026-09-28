// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../plugin.hpp"
#include "../../render.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

auto describes(AUDIO::PLUGIN::Control &out) -> Flag {
  return GRAPH::described(
    VIEWS::SIGNALS::mapped(), VIEWS::SIGNALS::number(), out);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::laned() -> Whole {
  const Whole track = attended();
  if (track == NONE) return NONE;
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  for (Whole lane = 0; lane < lanes.size(); ++lane)
    if (lanes[lane].kind == KIND::CONTROL) return lane;
  return NONE;
}

auto SOUND::VIEWS::SIGNALS::mapped() -> String {
  const Whole lane = laned();
  return lane == NONE ? String() : RENDER::mapped(attended(), lane);
}

auto SOUND::VIEWS::SIGNALS::named() -> String {
  const String plugin = mapped();
  if (plugin.empty()) return {};
  const Whole paged = number();
  return paged < VIEWS::parameters(plugin) ? VIEWS::named(plugin, paged) : String();
}

auto SOUND::VIEWS::SIGNALS::unit() -> String {
  AUDIO::PLUGIN::Control published;
  return ::describes(published) ? published.unit : String();
}

auto SOUND::VIEWS::SIGNALS::delivered(Float value) -> Float {
  AUDIO::PLUGIN::Control published;
  return ::describes(published) ? PLUGIN::scaled(published, value) : value;
}

auto SOUND::VIEWS::SIGNALS::spoken(Float value) -> String {
  return std::format("{:g}", delivered(value));
}

auto SOUND::VIEWS::SIGNALS::worded() -> Vector<Word> {
  const String plugin = mapped();
  const Whole listed = plugin.empty() ? 0 : VIEWS::parameters(plugin);
  Vector<Word> run;
  for (Whole number = 0; number <= FULL; ++number)
    if (number < listed)
      run.push_back({.number = number, .name = VIEWS::named(plugin, number)});
  for (Whole number = 0; number <= FULL; ++number)
    if (number >= listed) run.push_back({.number = number});
  return run;
}

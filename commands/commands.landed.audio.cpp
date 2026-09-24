// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

struct Landing {
  Whole lane = 0;
  Vector<Vector<Float>> lanes;
};

auto laned(Whole track, Whole lane) -> Whole {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  if (lane < lanes.size() && lanes[lane].kind == KIND::AUDIO) return lane;
  return COMMANDS::landing(track, KIND::AUDIO);
}

auto onto(Vector<Landing> &landings, Whole lane) -> Landing & {
  for (Landing &held : landings)
    if (held.lane == lane) return held;
  landings.push_back({.lane = lane});
  return landings.back();
}

void summed(Vector<Vector<Float>> &into, const Vector<Vector<Float>> &from) {
  if (into.size() < from.size()) into.resize(from.size());
  for (Whole channel = 0; channel < from.size(); ++channel) {
    if (into[channel].size() < from[channel].size())
      into[channel].resize(from[channel].size(), 0.0f);
    for (Whole frame = 0; frame < from[channel].size(); ++frame)
      into[channel][frame] += from[channel][frame];
  }
}

auto mixed(const CONTROL::Take &take, Whole track) -> Vector<Landing> {
  Vector<Landing> landings;
  if (!take.heard.empty())
    ::summed(
      ::onto(landings, COMMANDS::landing(track, KIND::AUDIO)).lanes,
      take.heard);
  for (const CONTROL::Crossed &crossed : take.tapped)
    ::summed(
      ::onto(landings, ::laned(track, crossed.lane)).lanes, crossed.lanes);
  return landings;
}

void land(const CONTROL::Take &take, Whole track, const Landing &fall) {
  if (fall.lanes.empty() || fall.lanes[0].empty()) return;
  const Whole stock = INVENTORY::add(
    std::format("take-{}", INVENTORY::held().stocks.size()), KIND::AUDIO);
  if (stock == NONE) return;
  INVENTORY::take(stock, fall.lanes);
  const Whole frames = fall.lanes[0].size();
  COMMANDS::covered(track, fall.lane, take.at, take.at + frames);
  TIMELINE::place(
    INVENTORY::held(), track, fall.lane,
    {.stock = stock, .at = take.at, .from = 0, .frames = frames});
}

}  // namespace

void SOUND::COMMANDS::landed(const CONTROL::Take &take) {
  const Whole track = CONTROL::driven();
  if (track >= TIMELINE::held().tracks.size()) return;
  for (const Landing &fall : ::mixed(take, track)) ::land(take, track, fall);
}

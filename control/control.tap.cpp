// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

namespace {
using namespace SOUND;

void grown(Vector<Vector<Float>> &lanes, Whole end) {
  lanes.resize(CHANNELS);
  for (Vector<Float> &lane : lanes)
    if (lane.size() < end) lane.resize(end, 0.0f);
}

auto crossing(CONTROL::Take &take, Whole lane) -> CONTROL::Crossed & {
  for (CONTROL::Crossed &held : take.tapped)
    if (held.lane == lane) return held;
  take.tapped.push_back({.lane = lane});
  return take.tapped.back();
}

}  // namespace

auto SOUND::CONTROL::tap(
  Whole at, Whole lane, const Vector<Vector<Float>> &lanes) -> Whole {
  Live &live = standing();
  if (!live.taking || lanes.empty() || lanes[0].empty()) return 0;
  const Whole frames = lanes[0].size();
  if (at + frames <= live.take.at) return 0;
  const Whole from = at < live.take.at ? live.take.at - at : 0;
  const Whole into = at > live.take.at ? at - live.take.at : 0;
  Vector<Vector<Float>> &crossed = ::crossing(live.take, lane).lanes;
  ::grown(crossed, into + frames - from);
  for (Whole channel = 0; channel < CHANNELS && channel < lanes.size();
       ++channel)
    for (Whole frame = from; frame < frames; ++frame)
      crossed[channel][into + frame - from] += lanes[channel][frame];
  return frames - from;
}

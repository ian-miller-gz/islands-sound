// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "stretch.internal.hpp"

namespace {
using namespace SOUND;

auto resampled(const Vector<Float> &lane, Whole frames) -> Vector<Float> {
  Vector<Float> out(frames, 0.0f);
  if (lane.empty() || frames == 0) return out;
  const Float step = Float(lane.size()) / Float(frames);
  for (Whole at = 0; at < frames; ++at) {
    const Float place = Float(at) * step;
    const Whole low = std::min(Whole(place), lane.size() - 1);
    const Whole high = std::min(low + 1, lane.size() - 1);
    out[at] = lane[low] + (lane[high] - lane[low]) * (place - Float(low));
  }
  return out;
}

}  // namespace

auto SOUND::STRETCH::length(const Take &take) -> Whole {
  return take.lanes.empty() ? 0 : take.lanes[0].size();
}

auto SOUND::STRETCH::summed(const Take &take) -> Vector<Float> {
  Vector<Float> sum(length(take), 0.0f);
  for (const Vector<Float> &lane : take.lanes)
    for (Whole at = 0; at < std::min(sum.size(), lane.size()); ++at)
      sum[at] += lane[at];
  return sum;
}

auto SOUND::STRETCH::resampled(const Take &take, Whole frames) -> Take {
  Take made = {.path = take.path, .lanes = {}};
  for (const Vector<Float> &lane : take.lanes)
    made.lanes.push_back(::resampled(lane, frames));
  return made;
}

auto SOUND::STRETCH::stretched(const Take &take, Float ratio, Integer cents)
  -> Take {
  if (ratio == SAME && cents == 0) return take;
  const Float shift = std::exp2(Float(cents) / OCTAVE);
  const Whole frames =
    Whole(std::lround(std::max(Float(length(take)) * ratio, 0.0f)));
  return resampled(scaled(take, ratio * shift), frames);
}

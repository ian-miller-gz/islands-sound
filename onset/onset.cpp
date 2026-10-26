// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "onset.hpp"

namespace SOUND::ONSET {

namespace {

auto frames(const Take &take) -> Whole {
  Whole longest = 0;
  for (const auto &lane : take.lanes) longest = std::max(longest, lane.size());
  return longest;
}

auto energy(const Take &take, Whole from) -> Float {
  Float sum = 0.0f;
  for (Whole at = from; at < from + HOP; ++at) {
    Float sample = 0.0f;
    for (const auto &lane : take.lanes)
      if (at < lane.size()) sample += lane[at];
    sum += sample * sample;
  }
  return sum / HOP;
}

auto risen(const Take &take, Float &largest) -> Vector<Float> {
  Vector<Float> rises(frames(take) / HOP, 0.0f);
  Float before = 0.0f;
  largest = 0.0f;
  for (Whole hop = 0; hop < rises.size(); ++hop) {
    const Float now = energy(take, hop * HOP);
    rises[hop] = std::max(now - before, 0.0f);
    largest = std::max(largest, rises[hop]);
    before = now;
  }
  return rises;
}

auto peaked(const Vector<Float> &rises, Whole hop) -> Flag {
  const Float before = hop > 0 ? rises[hop - 1] : 0.0f;
  const Float after = hop + 1 < rises.size() ? rises[hop + 1] : 0.0f;
  return rises[hop] > before && rises[hop] > after;
}

}  // namespace

auto found(const Take &take, Float sense) -> Vector<Whole> {
  Float largest = 0.0f;
  const Vector<Float> rises = risen(take, largest);
  const Float held = std::clamp(sense, 0.0f, 1.0f);
  const Float bar = (FLOOR + (1.0f - held) * (1.0f - FLOOR)) * largest;
  Vector<Whole> onsets;
  for (Whole hop = 0; hop < rises.size(); ++hop) {
    const Whole at = hop * HOP;
    if (rises[hop] <= bar || !peaked(rises, hop)) continue;
    if (!onsets.empty() && at - onsets.back() < GAP) continue;
    onsets.push_back(at);
  }
  return onsets;
}

}  // namespace SOUND::ONSET

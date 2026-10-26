// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <limits>

#include "peaks.hpp"

namespace {
using namespace SOUND;
using PEAKS::GRAIN;
using PEAKS::Level;
using PEAKS::Peak;

constexpr Peak OPEN = {
  std::numeric_limits<Float>::max(), std::numeric_limits<Float>::lowest()};

auto fold(const Peak &peak, Float value) -> Peak {
  return {std::min(peak.least, value), std::max(peak.most, value)};
}

auto fold(const Peak &peak, const Peak &other) -> Peak {
  return {std::min(peak.least, other.least), std::max(peak.most, other.most)};
}

auto length(const Take &take) -> Whole {
  return take.lanes.empty() ? 0 : take.lanes[0].size();
}

auto grained(Whole frames) -> Whole { return (frames + GRAIN - 1) / GRAIN; }

auto scan(const Take &take, Whole from, Whole to, Peak peak) -> Peak {
  for (const Vector<Float> &lane : take.lanes)
    for (Whole at = from; at < std::min(to, lane.size()); ++at)
      peak = ::fold(peak, lane[at]);
  return peak;
}

auto scan(const Level &level, Whole from, Whole to, Peak peak) -> Peak {
  for (Whole at = from; at < to; ++at) peak = ::fold(peak, level.grains[at]);
  return peak;
}

}  // namespace

auto SOUND::PEAKS::level(const Take &take) -> Level {
  const Whole frames = ::length(take);
  Level level;
  level.grains.assign(::grained(frames), ::OPEN);
  for (const Vector<Float> &lane : take.lanes)
    for (Whole at = 0; at < std::min(frames, lane.size()); ++at)
      level.grains[at / GRAIN] = ::fold(level.grains[at / GRAIN], lane[at]);
  return level;
}

auto SOUND::PEAKS::peak(
  const Take &take, const Level &level, Whole from, Whole frames) -> Peak {
  const Whole length = ::length(take);
  const Whole begin = std::min(from, length);
  const Whole end = begin + std::min(frames, length - begin);
  if (begin >= end) return {};
  const Whole low = ::grained(begin);
  const Whole high = std::min(
    end == length ? ::grained(length) : end / GRAIN, level.grains.size());
  if (low >= high) return ::scan(take, begin, end, ::OPEN);
  const Peak head = ::scan(take, begin, low * GRAIN, ::OPEN);
  const Peak body = ::scan(level, low, high, head);
  return ::scan(take, std::min(high * GRAIN, end), end, body);
}

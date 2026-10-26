// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "contour.hpp"

namespace SOUND::CONTOUR {

namespace {

constexpr Float SAMPLED = static_cast<Float>(RATE) / DOWN;
constexpr Whole SPAN = WINDOW / DOWN;
constexpr Whole STRIDE = HOP / DOWN;
constexpr Whole SHORT = static_cast<Whole>(SAMPLED / HIGHEST);
constexpr Whole LONG = static_cast<Whole>(SAMPLED / LOWEST);
constexpr Whole SUM = SPAN - LONG;
constexpr Float HALF = 0.5f;

static_assert(SHORT > 0 && LONG < SPAN);

auto decimated(const Take &take) -> Vector<Float> {
  Whole frames = 0;
  for (const auto &lane : take.lanes) frames = std::max(frames, lane.size());
  Vector<Float> mono(frames / DOWN, 0.0f);
  for (const auto &lane : take.lanes)
    for (Whole at = 0; at < std::min(lane.size(), mono.size() * DOWN); ++at)
      mono[at / DOWN] += lane[at];
  for (auto &sample : mono) sample /= DOWN;
  return mono;
}

auto quiet(const Float *window) -> Flag {
  Float energy = 0.0f;
  for (Whole at = 0; at < SPAN; ++at) energy += window[at] * window[at];
  return energy / SPAN < QUIET;
}

void differ(const Float *window, Vector<Float> &curve) {
  curve[0] = 1.0f;
  Float running = 0.0f;
  for (Whole lag = 1; lag <= LONG; ++lag) {
    Float sum = 0.0f;
    for (Whole at = 0; at < SUM; ++at) {
      const Float gap = window[at] - window[at + lag];
      sum += gap * gap;
    }
    running += sum;
    curve[lag] = running > 0.0f ? sum * lag / running : 1.0f;
  }
}

auto dipped(const Vector<Float> &curve) -> Whole {
  Whole lag = SHORT;
  while (lag <= LONG && curve[lag] >= DIP) ++lag;
  if (lag > LONG) return NONE;
  while (lag < LONG && curve[lag + 1] < curve[lag]) ++lag;
  return lag;
}

auto stepped(const Vector<Float> &curve, Whole lag) -> Float {
  if (lag <= SHORT || lag >= LONG) return static_cast<Float>(lag);
  const Float before = curve[lag - 1], at = curve[lag], after = curve[lag + 1];
  const Float bend = before - at - at + after;
  if (bend <= 0.0f) return static_cast<Float>(lag);
  return lag + HALF * (before - after) / bend;
}

auto pitched(const Float *window, Vector<Float> &curve) -> Float {
  if (quiet(window)) return 0.0f;
  differ(window, curve);
  const Whole lag = dipped(curve);
  return lag == NONE ? 0.0f : SAMPLED / stepped(curve, lag);
}

}  // namespace

auto traced(const Take &take) -> Vector<Float> {
  const Vector<Float> mono = decimated(take);
  Vector<Float> curve(LONG + 1, 0.0f), trace;
  for (Whole at = 0; at + SPAN <= mono.size(); at += STRIDE)
    trace.push_back(pitched(mono.data() + at, curve));
  return trace;
}

}  // namespace SOUND::CONTOUR

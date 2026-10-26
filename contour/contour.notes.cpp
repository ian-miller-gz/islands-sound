// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "contour.hpp"

namespace SOUND::CONTOUR {

namespace {

constexpr Float TUNING = 440.0f;
constexpr Float REFERENCE = 69.0f;
constexpr Float CENTS = 100.0f;

struct Run {
  Whole first = 0;
  Vector<Float> pitches;
};

auto steps(Float hertz) -> Float {
  return REFERENCE + CLASSES * std::log2(hertz / TUNING);
}

auto steady(const Run &run, Float pitch) -> Flag {
  return !run.pitches.empty() &&
         std::abs(pitch - run.pitches.front()) * CENTS <= STEADY;
}

void closed(Run &run, Vector<Found> &founds) {
  auto &pitches = run.pitches;
  if (pitches.size() >= SHORTEST) {
    const auto middle = pitches.begin() + pitches.size() / 2;
    std::nth_element(pitches.begin(), middle, pitches.end());
    const Float pitch = std::round(*middle);
    founds.push_back(
      {.at = run.first * HOP,
       .frames = pitches.size() * HOP,
       .pitch = static_cast<Whole>(pitch),
       .cents = static_cast<Integer>(std::lround((*middle - pitch) * CENTS))});
  }
  pitches.clear();
}

}  // namespace

auto noted(const Vector<Float> &trace) -> Vector<Found> {
  Vector<Found> founds;
  Run run;
  run.pitches.reserve(trace.size());
  for (Whole hop = 0; hop < trace.size(); ++hop) {
    const Float pitch = trace[hop] > 0.0f ? steps(trace[hop]) : 0.0f;
    if (pitch > 0.0f && steady(run, pitch)) {
      run.pitches.push_back(pitch);
      continue;
    }
    closed(run, founds);
    run.first = hop;
    if (pitch > 0.0f) run.pitches.push_back(pitch);
  }
  closed(run, founds);
  return founds;
}

}  // namespace SOUND::CONTOUR

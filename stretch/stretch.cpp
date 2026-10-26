// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <numbers>

#include "stretch.internal.hpp"

namespace {
using namespace SOUND;
using STRETCH::SEEK;
using STRETCH::WINDOW;

constexpr Whole HOP = WINDOW / 2;
constexpr Float FLOOR = 1.0e-6f;

struct Laying {
  const Take *source = nullptr;
  Vector<Float> mono, hann, blend, weights;
  Take out;
  Integer last = 0;
};

auto read(const Vector<Float> &lane, Integer at) -> Float {
  return at >= 0 && Whole(at) < lane.size() ? lane[at] : 0.0f;
}

auto hann() -> Vector<Float> {
  Vector<Float> window(WINDOW);
  for (Whole at = 0; at < WINDOW; ++at) {
    const Float bend =
      std::sin(std::numbers::pi_v<Float> * Float(at) / Float(WINDOW));
    window[at] = bend * bend;
  }
  return window;
}

auto score(const Laying &laying, Whole from, Integer at) -> Float {
  Float sum = 0.0f;
  for (Whole step = 0; step < HOP; ++step)
    sum += laying.blend[from + step] * ::read(laying.mono, at + Integer(step));
  return sum;
}

auto seek(const Laying &laying, Whole from, Integer at) -> Integer {
  Integer best = std::clamp(at, Integer(0), laying.last);
  Float most = ::score(laying, from, best);
  for (Integer shift = -Integer(SEEK); shift <= Integer(SEEK); ++shift) {
    const Integer candidate = std::clamp(at + shift, Integer(0), laying.last);
    const Float sum = ::score(laying, from, candidate);
    if (sum <= most) continue;
    most = sum;
    best = candidate;
  }
  return best;
}

void lay(Laying &laying, Whole from, Integer at) {
  const Take &source = *laying.source;
  for (Whole lane = 0; lane < source.lanes.size(); ++lane)
    for (Whole step = 0; step < WINDOW; ++step)
      laying.out.lanes[lane][from + step] +=
        laying.hann[step] * ::read(source.lanes[lane], at + Integer(step));
  for (Whole step = 0; step < WINDOW; ++step) {
    laying.blend[from + step] +=
      laying.hann[step] * ::read(laying.mono, at + Integer(step));
    laying.weights[from + step] += laying.hann[step];
  }
}

auto settled(Laying &laying, Whole total) -> Take {
  for (Vector<Float> &lane : laying.out.lanes) {
    for (Whole at = 0; at < total; ++at)
      lane[at] =
        laying.weights[at] > FLOOR ? lane[at] / laying.weights[at] : 0.0f;
    lane.resize(total);
  }
  return std::move(laying.out);
}

}  // namespace

auto SOUND::STRETCH::scaled(const Take &take, Float scale) -> Take {
  const Whole frames = length(take);
  const Whole total = Whole(std::lround(std::max(Float(frames) * scale, 0.0f)));
  const Vector<Float> blank(total + WINDOW, 0.0f);
  Laying laying = {
    .source = &take,
    .mono = STRETCH::summed(take),
    .hann = ::hann(),
    .blend = blank,
    .weights = blank,
    .out = {take.path, {take.lanes.size(), blank}},
    .last = frames > WINDOW ? Integer(frames - WINDOW) : 0};
  for (Whole from = 0, index = 0; from < total; from += HOP, ++index)
    ::lay(
      laying, from,
      ::seek(laying, from, Integer(std::lround(Float(index * HOP) / scale))));
  return ::settled(laying, total);
}

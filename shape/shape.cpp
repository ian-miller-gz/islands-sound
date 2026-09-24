// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numbers>

#include "shape.doors.hpp"

namespace {

constexpr STRING::Hot NAMES[] = {"hold", "", "rise", "fall", "ease"};

constexpr Float BEND = 4.0f * std::numbers::ln2_v<Float>;

auto swelled(Float part) -> Float {
  return (std::exp(::BEND * part) - 1.0f) / (std::exp(::BEND) - 1.0f);
}

auto flattened(Float part) -> Float { return 1.0f - ::swelled(1.0f - part); }

auto eased(Float part) -> Float { return part * part * (3.0f - 2.0f * part); }

}  // namespace

auto SOUND::SHAPE::spoken(Whole shape) -> STRING::Hot {
  return shape < std::size(::NAMES) ? ::NAMES[shape] : "unknown";
}

auto SOUND::SHAPE::meant(const String &token) -> Whole {
  for (Whole shape = 0; shape < std::size(::NAMES); ++shape)
    if (token == ::NAMES[shape]) return shape;
  return NONE;
}

auto SOUND::SHAPE::reading(Whole shape, Float part) -> Float {
  const Float travelled = std::clamp(part, 0.0f, 1.0f);
  if (shape == HOLD) return travelled < 1.0f ? 0.0f : 1.0f;
  if (shape == RISE) return ::swelled(travelled);
  if (shape == FALL) return ::flattened(travelled);
  if (shape == EASE) return ::eased(travelled);
  return travelled;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr Float OFF = 0.0f, ON = 1.0f;
constexpr Whole STEPS = 1;

auto faced() -> LANE::Face& { return LANE::faced(WAVE::laned()); }

namespace FLAG {

constexpr STRING::Hot NAME = "Onsets";

auto published() -> AUDIO::PLUGIN::Control {
  return {.steps = ::STEPS, .least = ::OFF, .most = ::ON};
}

auto read() -> Float { return ::faced().onsets != 0 ? ::ON : ::OFF; }

void write(Float value) { ::faced().onsets = Whole(std::lround(value)); }

}  // namespace FLAG

namespace SENSE {

constexpr STRING::Hot NAME = "Sense";

auto published() -> AUDIO::PLUGIN::Control {
  return {.resting = ONSET::RESTING, .least = ::OFF, .most = ::ON};
}

auto read() -> Float { return ::faced().sense; }

void write(Float value) { ::faced().sense = std::clamp(value, ::OFF, ::ON); }

}  // namespace SENSE

}  // namespace

auto SOUND::VIEWS::WAVE::ONSETS::flag() -> RIG::Source {
  return {
    .name = ::FLAG::NAME,
    .published = ::FLAG::published,
    .read = ::FLAG::read,
    .write = ::FLAG::write};
}

auto SOUND::VIEWS::WAVE::ONSETS::sense() -> RIG::Source {
  return {
    .name = ::SENSE::NAME,
    .published = ::SENSE::published,
    .read = ::SENSE::read,
    .write = ::SENSE::write};
}

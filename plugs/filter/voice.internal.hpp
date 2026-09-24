// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "filter.hpp"

namespace SOUND::FILTER {

constexpr Whole CORNERS = 64;
constexpr Float STEP = (HIGHEST - LOWEST) / Float(CORNERS);

struct Lane {
  Float band = 0;
  Float low = 0;
};

struct Sieve {
  Whole rate = 0;
  Whole channels = 0;
  Vector<Float> corners;
  Float rows[PARAMETERS] = {};
  Whole mode = LOW;
  Float loop = 0;
  Float feed = 0;
  Float carry = 0;
  Float damping = 0;
  Vector<Lane> lanes;
  Float peak[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

void bake(Sieve &sieve);

void settle(Sieve &sieve);

void apply(Sieve &sieve, const AUDIO::PLUGIN::Event &event);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::FILTER

namespace SOUND::FILTER::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::FILTER::SURFACE

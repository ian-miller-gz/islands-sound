// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cstdint>

#include <threads.hpp>

#include "percussive.hpp"

namespace SOUND::PERCUSSIVE {

constexpr Whole TABLE = 1024;
constexpr Whole SHIFT = 22;
static_assert(TABLE == 1u << (32 - SHIFT));

struct Strike {
  uint32_t phase = 0;
  Float step = 0;
  Float rest = 0;
  Float bend = 0;
  Float level = 0;
  Float drop = 0;
  uint32_t noise = 0;
  Float held = 0;
  Float velocity = 0;
};

struct Machine {
  Whole rate = 0;
  Whole channels = 0;
  Vector<Float> wave;
  Float wheel = 0;
  Float gain = 0;
  Float choke = 0;
  Float levels[SLOTS] = {};
  Float decays[SLOTS] = {};
  Strike strikes[SLOTS];
  Float level[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

auto delta(Float seconds, Whole rate) -> Float;

void apply(Machine &machine, const AUDIO::PLUGIN::Event &event);

void steer(Machine &machine, Whole id, Float value);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::PERCUSSIVE

namespace SOUND::PERCUSSIVE::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::PERCUSSIVE::SURFACE

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cstdint>

#include <threads.hpp>

#include "additive.hpp"

namespace SOUND::ADDITIVE {

constexpr Whole TABLE = 4096;
constexpr Whole TURN = 20;

enum Stage : Whole { IDLE, RISING, FALLING, HELD, LEAVING };

struct Note {
  uint32_t phase = 0;
  uint32_t step = 0;
  Whole pitch = 0;
  Float velocity = 0;
  Float level = 0;
  Whole stage = IDLE;
  Whole struck = 0;
};

struct Synth {
  Whole rate = 0;
  Whole channels = 0;
  Vector<Float> table;
  Vector<uint32_t> steps;
  Float amplitudes[PARTIALS] = {};
  Float span = 1;
  Float gain = 0;
  Float attack = 0;
  Float decay = 0;
  Float release = 0;
  Float sustain = 0;
  Float rise = 0;
  Float fall = 0;
  Float drop = 0;
  Note notes[VOICES];
  Whole clock = 0;
  Float level[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

auto delta(Float seconds, Whole rate) -> Float;

void shape(Synth &synth);

void apply(Synth &synth, const AUDIO::PLUGIN::Event &event);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::ADDITIVE

namespace SOUND::ADDITIVE::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::ADDITIVE::SURFACE

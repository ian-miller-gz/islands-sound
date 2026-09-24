// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cstdint>

#include <threads.hpp>

#include "subtractive.hpp"

namespace SOUND::SUBTRACTIVE {

constexpr Whole CENTS = 25;

enum Stage : Whole { IDLE, RISING, FALLING, HELD, LEAVING };

enum Curve : Whole { LOUDNESS, CONTOUR, CURVES };

struct Envelope {
  Float attack = 0;
  Float decay = 0;
  Float sustain = 0;
  Float release = 0;
  Float rise = 0;
  Float fall = 0;
  Float drop = 0;
};

struct Gate {
  Float level = 0;
  Whole stage = IDLE;
};

struct Note {
  uint32_t phases[OSCILLATORS] = {};
  uint32_t steps[OSCILLATORS] = {};
  Whole pitch = 0;
  Float velocity = 0;
  Float low = 0;
  Float band = 0;
  Gate gates[CURVES];
  Whole struck = 0;
};

struct Synth {
  Whole rate = 0;
  Whole channels = 0;
  Vector<uint32_t> steps;
  Vector<Float> ratios;
  Float turn = 0;
  Float gain = 0;
  Whole shape = SAW;
  Float spread = 0;
  Float detune = 1;
  Float cutoff = 0;
  Float resonance = 0;
  Float damping = 2;
  Float depth = 0;
  Envelope envelopes[CURVES];
  Note notes[VOICES];
  Whole clock = 0;
  Float level[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

auto delta(Float seconds, Whole rate) -> Float;

void shape(Envelope &envelope, Whole rate);

void apply(Synth &synth, const AUDIO::PLUGIN::Event &event);

void steer(Synth &synth, Whole id, Float value);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::SUBTRACTIVE

namespace SOUND::SUBTRACTIVE::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::SUBTRACTIVE::SURFACE

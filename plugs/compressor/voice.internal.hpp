// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "compressor.hpp"

namespace SOUND::COMPRESSOR {

constexpr Whole DECIBELS = Whole(LOUDEST - QUIETEST);

struct Press {
  Whole rate = 0;
  Whole channels = 0;
  Vector<Float> levels;
  Float rows[PARAMETERS] = {};
  Float threshold = 0;
  Float makeup = 0;
  Float rising = 0;
  Float falling = 0;
  Whole folds = 0;
  Float envelope = 0;
  Float peak[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

void bake(Press &press);

void settle(Press &press);

void apply(Press &press, const AUDIO::PLUGIN::Event &event);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::COMPRESSOR

namespace SOUND::COMPRESSOR::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::COMPRESSOR::SURFACE

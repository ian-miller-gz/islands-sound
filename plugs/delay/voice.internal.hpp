// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "delay.hpp"

namespace SOUND::DELAY {

struct Lane {
  Vector<Float> ring;
  Whole at = 0;
};

struct Trail {
  Whole rate = 0;
  Whole channels = 0;
  Whole slots = 0;
  Float rows[PARAMETERS] = {};
  Whole spacing = 0;
  Float given = 0;
  Float wet = 0;
  Float dry = 0;
  Vector<Lane> lanes;
  Float peak[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

void stretch(Trail &trail);

void settle(Trail &trail);

void apply(Trail &trail, const AUDIO::PLUGIN::Event &event);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::DELAY

namespace SOUND::DELAY::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::DELAY::SURFACE

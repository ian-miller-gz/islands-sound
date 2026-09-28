// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "../../plugin.hpp"

namespace SOUND::KEYS {

constexpr STRING::Hot LOWER = "ZSXDCVGBHNJM";
constexpr STRING::Hot UPPER = "Q2W3ER5T6Y7U";
constexpr Whole SEMITONES = 12;
constexpr Whole SPAN = SEMITONES * 2;
constexpr Whole HIGHEST = 127;

constexpr Float FORCE = 0.8f;

constexpr Whole OCTAVE = 0;
constexpr Whole PARAMETERS = 1;
constexpr Float LEAST = 0.0f;
constexpr Float MOST = 8.0f;
constexpr Float RESTING = 4.0f;

struct Ring {
  static constexpr Whole SIZE = 64;
  AUDIO::PLUGIN::Event slots[SIZE];
  THREADS::Shared<Whole> head{0};
  THREADS::Shared<Whole> tail{0};
  auto push(const AUDIO::PLUGIN::Event &edge) -> Flag;
  auto pop(AUDIO::PLUGIN::Event &edge) -> Flag;
};

struct Board {
  Float octave = RESTING;
  Flag down[SPAN] = {};
  Ring ring;
};

auto keyed(Whole place) -> Whole;
auto pitched(const Board &board, Whole place, Whole &pitch) -> Flag;

namespace SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SURFACE

}  // namespace SOUND::KEYS

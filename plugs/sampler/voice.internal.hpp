// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "sampler.hpp"

namespace SOUND::SAMPLER {

struct Source {
  String name;
  Vector<Vector<Float>> lanes;
};

struct Voice {
  Whole source = 0;
  Float place = 0;
  Float base = 0;
  Float step = 0;
  Float from = 0;
  Float level = 0;
  Float rise = 0;
  Float fall = 0;
  Float velocity = 0;
  Whole pitch = 0;
  Flag gated = false;
  Flag live = false;
};

struct Player {
  Whole rate = 0;
  Whole channels = 0;
  Float ratio = 1;
  Vector<Source> sources;
  Vector<Float> steps;
  Vector<Float> cents;
  Float gain = 0;
  Whole chosen = 0;
  Float root = 0;
  Float tune = 0;
  Float tuning = 1;
  Float lowest = 0;
  Float start = 0;
  Float loop = 0;
  Float attack = 0;
  Float release = 0;
  Voice voices[VOICES];
  Float level[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

void stock(Player &player);

auto frames(const Source &source) -> Whole;

auto span(const Player &player) -> Float;

auto delta(Float seconds, Whole rate) -> Float;

void apply(Player &player, const AUDIO::PLUGIN::Event &event);

void steer(Player &player, Whole id, Float value);

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count);

}  // namespace SOUND::SAMPLER

namespace SOUND::SAMPLER::SURFACE {
auto parameters(void *instance) -> Whole;
auto name(void *instance, Whole index) -> String;
auto reading(void *instance, Whole index) -> String;
auto held(void *instance, Whole index) -> Float;
auto control(void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag;
}  // namespace SOUND::SAMPLER::SURFACE

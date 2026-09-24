// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../graph.hpp"
#include "../timeline.hpp"
#include "render.hpp"

namespace SOUND::RENDER {

auto worded(Whole track, Whole lane, Whole number, Float value) -> Float;

struct Wave {
  Vector<Vector<AUDIO::PLUGIN::Sample>> lanes;
  Vector<Vector<Vector<AUDIO::PLUGIN::Sample>>> ins;
  Vector<AUDIO::PLUGIN::Event> events;
  Vector<Vector<AUDIO::PLUGIN::Event>> outs;
};

struct Pass {
  Walk walk;
  Whole frames = 0;
  Flag jumped = false;
  Flag swapped = false;
};

struct Leg {
  Whole start = 0;
  Whole offset = 0;
  Whole frames = 0;
};

constexpr Whole LEGS = 2;

auto legs(const Walk &walk, Whole frames, Leg (&out)[LEGS]) -> Whole;

auto carried() -> Vector<Wave> &;

auto delivery() -> Whole &;

void gather(Whole node, const Pass &pass);

void rooted(Whole node, const Pass &pass);

namespace STRUCK {

void struck(const String &root, Whole out, const AUDIO::PLUGIN::Event &edge);

void lifted(const String &root, Whole out, Whole pitch);

void cut(
  const String &root, Whole out, Whole at, Vector<AUDIO::PLUGIN::Event> &into);

void swapped(
  const String &root, Whole out, Whole start,
  Vector<AUDIO::PLUGIN::Event> &into);

auto jumped(const Walk &walk, Whole frames) -> Flag;

}  // namespace STRUCK

auto threading() -> Flag;

auto claims() -> Vector<String>;

auto threaded() -> Whole &;

auto moved() -> Whole;

namespace TAP {

void adopt();
void fed(Whole start, Whole frames);

}  // namespace TAP

}  // namespace SOUND::RENDER

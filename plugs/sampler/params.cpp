// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "sampler.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot SWITCHES[] = {"Off", "On"};

constexpr Float DETUNE = 50;

constexpr Float HIGHEST = 127;

struct Row {
  STRING::Hot name;
  STRING::Hot unit;
  Float resting;
  Float least;
  Float most;
  Whole steps;
};

constexpr Row ROWS[] = {
  {"Gain", "", 0.80f, 0, 1, 0},    {"Stock", "", 0, 0, 0, 0},
  {"Root", "", 60, 0, HIGHEST, 0}, {"Tune", "ct", 0, -DETUNE, DETUNE, 0},
  {"Start", "ms", 0, 0, 0, 0},     {"Loop", "", 0, 0, 1, 1},
  {"Attack", "s", 0.01f, 0, 1, 0}, {"Release", "s", 0.20f, 0.005f, 2, 0},
};
static_assert(sizeof(ROWS) / sizeof(ROWS[0]) == SAMPLER::PARAMETERS);

auto snapped(const Row &found, Float value) -> Float {
  const Float span = (found.most - found.least) / Float(found.steps);
  const Float places = (value - found.least) / span + 0.5f;
  return found.least + span * Float(Whole(places < 0 ? 0 : places));
}

}  // namespace

auto SOUND::SAMPLER::label(Whole index) -> String {
  return index < PARAMETERS ? String(ROWS[index].name) : String();
}

auto SOUND::SAMPLER::control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  if (index >= PARAMETERS) return false;
  const Row &found = ROWS[index];
  Vector<String> positions;
  if (found.steps != 0) positions = {SWITCHES, SWITCHES + found.steps + 1};
  out = {found.unit,    found.steps, positions,
         found.resting, found.least, found.most};
  return true;
}

auto SOUND::SAMPLER::resting(Whole index) -> Float {
  return index < PARAMETERS ? ROWS[index].resting : 0;
}

auto SOUND::SAMPLER::clamped(Whole index, Float value) -> Float {
  if (index >= PARAMETERS) return value;
  const Row &found = ROWS[index];
  if (found.most <= found.least) return value < 0 ? 0 : value;
  const Float held = value < found.least  ? found.least
                     : value > found.most ? found.most
                                          : value;
  return found.steps == 0 ? held : ::snapped(found, held);
}

auto SOUND::SAMPLER::notation(Whole index, Float value) -> String {
  if (index >= PARAMETERS) return {};
  const Row &found = ROWS[index];
  if (found.steps != 0) return String(SWITCHES[Whole(clamped(index, value))]);
  char buffer[24];
  std::snprintf(buffer, sizeof(buffer), "%.2f%s", value, found.unit);
  return buffer;
}

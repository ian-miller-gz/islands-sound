// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "subtractive.hpp"

namespace {

constexpr Float QUICKEST = 0.001f;
constexpr Float SLOWEST = 4.001f;

constexpr Float COARSE = 100;

struct Row {
  STRING::Hot name;
  STRING::Hot unit;
  Float resting;
  Float least;
  Float most;
  Whole steps;
};

constexpr Row TABLE[] = {
  {"Gain", "", 0.30f, 0, 1, 0},
  {"Shape", "", SOUND::SUBTRACTIVE::SAW, SOUND::SUBTRACTIVE::SAW,
   SOUND::SUBTRACTIVE::TRIANGLE, 2},
  {"Spread", "ct", 7.0f, 0, 25, 0},
  {"Cutoff", "Hz", 1200, 20, 12000, 0},
  {"Resonance", "", 0.30f, 0, 0.95f, 0},
  {"Depth", "Hz", 2400, 0, 8000, 0},
  {"Sweep", "s", 0.011f, QUICKEST, SLOWEST, 0},
  {"Close", "s", 0.401f, QUICKEST, SLOWEST, 0},
  {"Attack", "s", 0.011f, QUICKEST, SLOWEST, 0},
  {"Decay", "s", 0.1946f, QUICKEST, SLOWEST, 0},
  {"Sustain", "", 0.70f, 0, 1, 0},
  {"Release", "s", 0.251f, QUICKEST, SLOWEST, 0}};
static_assert(
  sizeof(TABLE) / sizeof(TABLE[0]) == SOUND::SUBTRACTIVE::PARAMETERS);

constexpr STRING::Hot WAVES[] = {"Saw", "Pulse", "Triangle"};

auto row(Whole index, Row &out) -> Flag {
  if (index >= SOUND::SUBTRACTIVE::PARAMETERS) return false;
  out = TABLE[index];
  return true;
}

auto snapped(const Row &found, Float value) -> Float {
  const Float span = (found.most - found.least) / Float(found.steps);
  const Float places = (value - found.least) / span + 0.5f;
  return found.least + span * Float(Whole(places < 0 ? 0 : places));
}

}  // namespace

auto SOUND::SUBTRACTIVE::label(Whole index) -> String {
  Row found;
  return ::row(index, found) ? String(found.name) : String();
}

auto SOUND::SUBTRACTIVE::control(Whole index, AUDIO::PLUGIN::Control &out)
  -> Flag {
  Row found;
  if (!::row(index, found)) return false;
  Vector<String> positions;
  if (found.steps != 0) positions = {WAVES, WAVES + found.steps + 1};
  out = {found.unit,    found.steps, positions,
         found.resting, found.least, found.most};
  return true;
}

auto SOUND::SUBTRACTIVE::resting(Whole index) -> Float {
  Row found;
  return ::row(index, found) ? found.resting : 0;
}

auto SOUND::SUBTRACTIVE::clamped(Whole index, Float value) -> Float {
  Row found;
  if (!::row(index, found)) return value;
  const Float held = value < found.least  ? found.least
                     : value > found.most ? found.most
                                          : value;
  return found.steps == 0 ? held : ::snapped(found, held);
}

auto SOUND::SUBTRACTIVE::notation(Whole index, Float value) -> String {
  Row found;
  if (!::row(index, found)) return {};
  if (found.steps != 0) return String(WAVES[Whole(clamped(index, value))]);
  char buffer[24];
  const char *shape = found.most >= COARSE ? "%.0f%s" : "%.2f%s";
  std::snprintf(buffer, sizeof(buffer), shape, value, found.unit);
  return buffer;
}

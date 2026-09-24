// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "additive.hpp"

namespace {

constexpr Float QUICKEST = 0.001f;
constexpr Float SLOWEST = 4.001f;

struct Row {
  STRING::Hot name;
  STRING::Hot unit;
  Float resting;
  Float least;
  Float most;
};

constexpr Row TABLE[] = {
  {"Gain", "", 0.30f, 0, 1},
  {"Attack", "s", 0.011f, QUICKEST, SLOWEST},
  {"Decay", "s", 0.1946f, QUICKEST, SLOWEST},
  {"Sustain", "", 0.70f, 0, 1},
  {"Release", "s", 0.251f, QUICKEST, SLOWEST}};
constexpr Whole SHAPED = sizeof(TABLE) / sizeof(TABLE[0]);
static_assert(SHAPED == SOUND::ADDITIVE::PARTIAL);

auto harmonic(Whole index) -> Row {
  return {"", "", 1.0f / Float(index - SHAPED + 1), 0, 1};
}

auto row(Whole index, Row &out) -> Flag {
  if (index >= SOUND::ADDITIVE::PARAMETERS) return false;
  out = index < SHAPED ? TABLE[index] : harmonic(index);
  return true;
}

}  // namespace

auto SOUND::ADDITIVE::label(Whole index) -> String {
  if (index >= PARAMETERS) return {};
  if (index < SHAPED) return String(TABLE[index].name);
  return "Harmonic " + std::to_string(index - SHAPED + 1);
}

auto SOUND::ADDITIVE::control(Whole index, AUDIO::PLUGIN::Control &out)
  -> Flag {
  Row found;
  if (!::row(index, found)) return false;
  out = {
    .unit = found.unit,
    .resting = found.resting,
    .least = found.least,
    .most = found.most};
  return true;
}

auto SOUND::ADDITIVE::resting(Whole index) -> Float {
  Row found;
  return ::row(index, found) ? found.resting : 0;
}

auto SOUND::ADDITIVE::clamped(Whole index, Float value) -> Float {
  Row found;
  if (!::row(index, found)) return value;
  return value < found.least  ? found.least
         : value > found.most ? found.most
                              : value;
}

auto SOUND::ADDITIVE::notation(Whole index, Float value) -> String {
  Row found;
  if (!::row(index, found)) return {};
  char buffer[24];
  std::snprintf(buffer, sizeof(buffer), "%.2f%s", value, found.unit);
  return buffer;
}

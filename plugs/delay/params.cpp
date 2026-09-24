// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "delay.hpp"

namespace {
using namespace SOUND;

struct Row {
  STRING::Hot label;
  STRING::Hot unit;
  Float resting;
  Float least;
  Float most;
};

constexpr Row ROWS[] = {
  {"Time", "ms", DELAY::SPACED, DELAY::BRIEFEST, DELAY::LONGEST},
  {"Feedback", "%", DELAY::FEW, DELAY::NOTHING, DELAY::DEEPEST},
  {"Mix", "%", DELAY::SHADED, DELAY::NOTHING, DELAY::WHOLLY}};
static_assert(sizeof(ROWS) / sizeof(ROWS[0]) == DELAY::PARAMETERS);

constexpr Whole CONTINUOUS = 0;

auto row(Whole index, Row &out) -> Flag {
  if (index >= DELAY::PARAMETERS) return false;
  out = ROWS[index];
  return true;
}

}  // namespace

auto SOUND::DELAY::control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  Row found;
  if (!::row(index, found)) return false;
  out = {
    .unit = found.unit,
    .steps = CONTINUOUS,
    .resting = found.resting,
    .least = found.least,
    .most = found.most};
  return true;
}

auto SOUND::DELAY::resting(Whole index) -> Float {
  Row found;
  return ::row(index, found) ? found.resting : 0;
}

auto SOUND::DELAY::clamped(Whole index, Float value) -> Float {
  Row found;
  if (!::row(index, found)) return value;
  return value < found.least  ? found.least
         : value > found.most ? found.most
                              : value;
}

auto SOUND::DELAY::notation(Whole index, Float value) -> String {
  Row found;
  if (!::row(index, found)) return {};
  char text[32];
  std::snprintf(text, sizeof(text), "%.3f%s", double(value), found.unit);
  return text;
}

auto SOUND::DELAY::label(Whole index) -> String {
  Row found;
  return ::row(index, found) ? String(found.label) : String();
}

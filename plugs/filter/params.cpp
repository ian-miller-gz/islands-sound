// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "filter.hpp"

namespace {
using namespace SOUND;

struct Row {
  STRING::Hot label;
  STRING::Hot unit;
  Float resting;
  Float least;
  Float most;
  Whole steps;
};

constexpr Row ROWS[] = {
  {"Mode", "", Float(FILTER::LOW), Float(FILTER::LOW), Float(FILTER::HIGH),
   FILTER::MODES - 1},
  {"Cutoff", "Hz", FILTER::HIGHEST, FILTER::LOWEST, FILTER::HIGHEST, 0},
  {"Resonance", "", FILTER::CALM, FILTER::CALM, FILTER::SHARPEST, 0}};
static_assert(sizeof(ROWS) / sizeof(ROWS[0]) == FILTER::PARAMETERS);

constexpr STRING::Hot CHOICES[] = {"Low", "Band", "High"};
static_assert(sizeof(CHOICES) / sizeof(CHOICES[0]) == FILTER::MODES);

auto row(Whole index, Row &out) -> Flag {
  if (index >= FILTER::PARAMETERS) return false;
  out = ROWS[index];
  return true;
}

auto snapped(const Row &found, Float value) -> Float {
  const Float span = (found.most - found.least) / Float(found.steps);
  const Float places = (value - found.least) / span + 0.5f;
  return found.least + span * Float(Whole(places < 0 ? 0 : places));
}

}  // namespace

auto SOUND::FILTER::control(Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  Row found;
  if (!::row(index, found)) return false;
  Vector<String> positions;
  if (found.steps != 0) positions = {CHOICES, CHOICES + found.steps + 1};
  out = {found.unit,    found.steps, positions,
         found.resting, found.least, found.most};
  return true;
}

auto SOUND::FILTER::resting(Whole index) -> Float {
  Row found;
  return ::row(index, found) ? found.resting : 0;
}

auto SOUND::FILTER::clamped(Whole index, Float value) -> Float {
  Row found;
  if (!::row(index, found)) return value;
  const Float held = value < found.least  ? found.least
                     : value > found.most ? found.most
                                          : value;
  return found.steps == 0 ? held : ::snapped(found, held);
}

auto SOUND::FILTER::notation(Whole index, Float value) -> String {
  Row found;
  if (!::row(index, found)) return {};
  if (found.steps != 0) return CHOICES[Whole(clamped(index, value))];
  char text[32];
  std::snprintf(text, sizeof(text), "%.3f%s", double(value), found.unit);
  return text;
}

auto SOUND::FILTER::label(Whole index) -> String {
  Row found;
  return ::row(index, found) ? String(found.label) : String();
}

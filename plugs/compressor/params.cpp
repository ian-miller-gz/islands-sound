// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdio>

#include "compressor.hpp"

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
  {"Threshold", "dB", COMPRESSOR::FULL, COMPRESSOR::QUIETEST, COMPRESSOR::FULL,
   0},
  {"Ratio", "", Float(COMPRESSOR::HALVED), Float(COMPRESSOR::HALVED),
   Float(COMPRESSOR::LIMIT), COMPRESSOR::RATIOS - 1},
  {"Attack", "ms", COMPRESSOR::BITING, COMPRESSOR::QUICKEST,
   COMPRESSOR::SLOWEST, 0},
  {"Release", "ms", COMPRESSOR::BREATHING, COMPRESSOR::QUICKEST,
   COMPRESSOR::SLOWEST, 0},
  {"Makeup", "dB", COMPRESSOR::FULL, COMPRESSOR::FULL, COMPRESSOR::LOUDEST, 0}};
static_assert(sizeof(ROWS) / sizeof(ROWS[0]) == COMPRESSOR::PARAMETERS);

constexpr STRING::Hot CHOICES[] = {"2:1", "4:1", "8:1", "Limit"};
static_assert(sizeof(CHOICES) / sizeof(CHOICES[0]) == COMPRESSOR::RATIOS);

auto row(Whole index, Row &out) -> Flag {
  if (index >= COMPRESSOR::PARAMETERS) return false;
  out = ROWS[index];
  return true;
}

auto snapped(const Row &found, Float value) -> Float {
  const Float span = (found.most - found.least) / Float(found.steps);
  const Float places = (value - found.least) / span + 0.5f;
  return found.least + span * Float(Whole(places < 0 ? 0 : places));
}

}  // namespace

auto SOUND::COMPRESSOR::control(Whole index, AUDIO::PLUGIN::Control &out)
  -> Flag {
  Row found;
  if (!::row(index, found)) return false;
  Vector<String> positions;
  if (found.steps != 0) positions = {CHOICES, CHOICES + found.steps + 1};
  out = {found.unit,    found.steps, positions,
         found.resting, found.least, found.most};
  return true;
}

auto SOUND::COMPRESSOR::resting(Whole index) -> Float {
  Row found;
  return ::row(index, found) ? found.resting : 0;
}

auto SOUND::COMPRESSOR::clamped(Whole index, Float value) -> Float {
  Row found;
  if (!::row(index, found)) return value;
  const Float held = value < found.least  ? found.least
                     : value > found.most ? found.most
                                          : value;
  return found.steps == 0 ? held : ::snapped(found, held);
}

auto SOUND::COMPRESSOR::notation(Whole index, Float value) -> String {
  Row found;
  if (!::row(index, found)) return {};
  if (found.steps != 0) return CHOICES[Whole(clamped(index, value))];
  char text[32];
  std::snprintf(text, sizeof(text), "%.3f%s", double(value), found.unit);
  return text;
}

auto SOUND::COMPRESSOR::label(Whole index) -> String {
  Row found;
  return ::row(index, found) ? String(found.label) : String();
}

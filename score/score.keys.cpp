// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cctype>

#include "score.doors.hpp"

namespace SOUND::SCORE {
namespace {

struct Pattern {
  STRING::Hot name;
  Vector<Whole> steps;
};

auto patterns() -> const Vector<Pattern> & {
  static const Vector<Pattern> table = {{"major", {0, 2, 4, 5, 7, 9, 11}},
                                        {"minor", {0, 2, 3, 5, 7, 8, 10}},
                                        {"harmonic", {0, 2, 3, 5, 7, 8, 11}},
                                        {"melodic", {0, 2, 3, 5, 7, 9, 11}},
                                        {"dorian", {0, 2, 3, 5, 7, 9, 10}},
                                        {"phrygian", {0, 1, 3, 5, 7, 8, 10}},
                                        {"lydian", {0, 2, 4, 6, 7, 9, 11}},
                                        {"mixolydian", {0, 2, 4, 5, 7, 9, 10}},
                                        {"locrian", {0, 1, 3, 5, 6, 8, 10}},
                                        {"pentatonic", {0, 2, 4, 7, 9}},
                                        {"blues", {0, 3, 5, 6, 7, 10}}};
  return table;
}

auto stepped(const Key &key) -> const Vector<Whole> * {
  for (const Pattern &row : patterns())
    if (key.scale == row.name) return &row.steps;
  return nullptr;
}

auto placed(const Key &key, Whole pitch) -> Whole {
  return (pitch + CLASSES - key.tonic % CLASSES) % CLASSES;
}

constexpr Whole NATURALS[] = {9, 11, 0, 2, 4, 5, 7};
constexpr STRING::Hot NAMES[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                 "F#", "G",  "G#", "A",  "A#", "B"};

constexpr Char FIRST = 'A', LAST = 'G';
constexpr STRING::Hot SHARP = "#", FLAT = "b";
constexpr Whole SEMITONE = 1;

}  // namespace
}  // namespace SOUND::SCORE

auto SOUND::SCORE::scales() -> Vector<STRING::Hot> {
  Vector<STRING::Hot> names;
  for (const Pattern &row : patterns()) names.push_back(row.name);
  return names;
}

auto SOUND::SCORE::degree(const Key &key, Whole pitch) -> Whole {
  const Vector<Whole> *steps = stepped(key);
  if (steps == nullptr) return NONE;
  const Whole place = placed(key, pitch);
  for (Whole step = 0; step < steps->size(); ++step)
    if ((*steps)[step] == place) return step + 1;
  return NONE;
}

auto SOUND::SCORE::snapped(const Key &key, Whole pitch) -> Whole {
  if (stepped(key) == nullptr) return pitch;
  for (Whole away = 0; away <= CLASSES; ++away) {
    if (away <= pitch && degree(key, pitch - away) != NONE) return pitch - away;
    if (degree(key, pitch + away) != NONE) return pitch + away;
  }
  return pitch;
}

auto SOUND::SCORE::classed(STRING::Hot name) -> Whole {
  const String word = name == nullptr ? String() : String(name);
  if (word.empty()) return NONE;
  const Char letter = static_cast<Char>(std::toupper(word.front()));
  if (letter < FIRST || letter > LAST) return NONE;
  const Whole natural = NATURALS[letter - FIRST];
  const String accidental = word.substr(1);
  if (accidental.empty()) return natural;
  if (accidental == SHARP) return (natural + SEMITONE) % CLASSES;
  if (accidental == FLAT) return (natural + CLASSES - SEMITONE) % CLASSES;
  return NONE;
}

auto SOUND::SCORE::classed(Whole tonic) -> STRING::Hot {
  return NAMES[tonic % CLASSES];
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "piano.heads.hpp"

namespace SOUND::VIEWS::ROLL {

namespace {

constexpr Drum DRUMS[] = {
  {52, 52, -3, CROSS, "", "China"},     {55, 55, -3, CROSS, "", "Splash"},
  {49, 49, -2, CROSS, "", "Crash"},     {57, 57, -2, CROSS, "", "Crash"},
  {42, 42, -1, CROSS, "+", "Hat"},      {46, 46, -1, CROSS, "o", "Open"},
  {51, 51, 0, CROSS, "", "Ride"},       {59, 59, 0, CROSS, "", "Ride"},
  {53, 53, 0, DIAMOND, "", "Bell"},     {48, 48, 1, PLAIN, "", "High"},
  {50, 50, 1, PLAIN, "", "High"},       {45, 45, 2, PLAIN, "", "Tom"},
  {47, 47, 2, PLAIN, "", "Tom"},        {38, 38, 3, PLAIN, "", "Snare"},
  {40, 40, 3, PLAIN, "", "Snare"},      {37, 37, 3, PLAIN, "", "Stick"},
  {41, 41, 5, PLAIN, "", "Floor"},      {43, 43, 5, PLAIN, "", "Floor"},
  {35, 36, 7, PLAIN, "", "Kick", true}, {44, 44, 9, CROSS, "", "Pedal", true},
};

constexpr Drum NONE = {};

}  // namespace

auto GET::drum(Whole note) -> const Drum * {
  for (const Drum &drum : DRUMS)
    if (note >= drum.from && note <= drum.to) return &drum;
  return nullptr;
}

auto GET::drums() -> Vector<AUDIO::PLUGIN::Voice> {
  Vector<AUDIO::PLUGIN::Voice> voices;
  for (const Drum &drum : DRUMS)
    if (std::ranges::none_of(voices, [&](const AUDIO::PLUGIN::Voice &voice) {
          return voice.name == drum.name;
        }))
      voices.push_back({drum.name, drum.to});
  return voices;
}

auto headed(Whole note) -> STRING::Hot {
  const Drum *drum = GET::drum(note);
  return (drum ? *drum : NONE).head;
}

auto marked(Whole note) -> STRING::Hot {
  const Drum *drum = GET::drum(note);
  return (drum ? *drum : NONE).mark;
}

}  // namespace SOUND::VIEWS::ROLL

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float WHOLE = 1.0f;
constexpr Float SECOND = 1000.0f;
constexpr Float DECIBEL = 1.1220185f;

auto stepped(Whole rate, Float milliseconds) -> Float {
  const Float frames = Float(rate) * milliseconds / SECOND;
  return frames <= WHOLE ? WHOLE : WHOLE / frames;
}

auto level(const COMPRESSOR::Press &press, Float decibels) -> Float {
  const Float place = decibels - COMPRESSOR::QUIETEST;
  const Whole whole = Whole(place < 0 ? 0 : place);
  if (whole >= COMPRESSOR::DECIBELS) return press.levels[COMPRESSOR::DECIBELS];
  const Float part = place - Float(whole);
  const Float step = press.levels[whole + 1] - press.levels[whole];
  return press.levels[whole] + step * part;
}

}  // namespace

void SOUND::COMPRESSOR::bake(Press &press) {
  press.levels.assign(DECIBELS + 1, 0);
  const Whole unity = Whole(FULL - QUIETEST);
  press.levels[unity] = WHOLE;
  for (Whole place = unity; place > 0; --place)
    press.levels[place - 1] = press.levels[place] / DECIBEL;
  for (Whole place = unity; place < DECIBELS; ++place)
    press.levels[place + 1] = press.levels[place] * DECIBEL;
}

void SOUND::COMPRESSOR::settle(Press &press) {
  press.threshold = ::level(press, press.rows[THRESHOLD]);
  press.makeup = ::level(press, press.rows[MAKEUP]);
  press.rising = ::stepped(press.rate, press.rows[ATTACK]);
  press.falling = ::stepped(press.rate, press.rows[RELEASE]);
  const Whole position = Whole(press.rows[RATIO]);
  press.folds = position >= LIMIT ? 0 : position + 1;
}

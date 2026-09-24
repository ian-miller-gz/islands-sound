// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND {

constexpr Whole NONE = static_cast<Whole>(-1);

constexpr Whole RATE = 48000;
constexpr Float TEMPO = 120.0f;
constexpr Whole METRE = 4;

constexpr Whole CHANNELS = 2;
constexpr Float SCALE = 32768.0f;

constexpr Whole PULSES = 960;

constexpr Whole CLASSES = 12;

struct Tempo {
  Whole at = 0;
  Float tempo = TEMPO;
};

struct Metre {
  Whole at = 0;
  Whole numerator = METRE, denominator = METRE;
};

struct Key {
  Whole at = 0;
  Whole tonic = 0;
  String scale;
};

struct Tag {
  Whole at = 0;
  String text;
};

struct Span {
  Whole from = 0, to = 0;
  String text;
};

struct Note {
  Whole pitch = 0;
  Whole at = 0;
  Whole length = 0;
  Float velocity = 0.0f;
};

struct Score {
  Vector<Note> notes;
};

}  // namespace SOUND

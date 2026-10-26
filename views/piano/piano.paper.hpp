// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL {

auto papered() -> Flag;

auto margin() -> Float;

namespace PAPER {

constexpr Float PLATE = 4.0f, INK = 5.0f;

void grid(const Vector<GUI::SAC::LADDER::Mark> &marks);

struct Bed {
  String id;
  Whole count = 0;
  STRING::Hot wash = "";
};

void range(const Bed &bed);

auto washed() -> Whole;

void state(SHELL::Session &session);

}  // namespace PAPER

namespace STAFF {

auto rule(Whole line) -> String;

void clef();

void lined(GUI::Handle page, const String &plate, Float wide, Whole count);

namespace SIGNS {

struct Glyph {
  GUI::Position at;
  Float wide = 0.0f, size = 0.0f;
};

auto named(Whole at) -> String;

void inked(
  GUI::Handle page, const String &id, STRING::Hot kind, Float depth,
  STRING::Hot ink);

void lettered(
  GUI::Handle page, const String &id, const STRING::Cold &text,
  const Glyph &glyph, Float down);

auto metred(GUI::Handle page, Float east, const Float (&lines)[2], Float down)
  -> Float;

}  // namespace SIGNS

namespace SHED {
void clef();
}  // namespace SHED

}  // namespace STAFF

}  // namespace SOUND::VIEWS::ROLL

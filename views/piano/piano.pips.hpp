// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr Float SPACE = 2.0f * STEP;
constexpr Float EM = 4.0f * SPACE;
constexpr Float BASELINE = 0.7974f;
struct Ink {
  Float west = 0.0f, wide = 0.0f, rise = 0.0f;
};
constexpr Ink INK = {0.05f, 0.31f, 0.135f};
constexpr Float LIFT = HALF * (STEP - SPACE);
constexpr Whole TIERS = 4;
constexpr Float GHOST = 0.75f;

auto tier(const Pip &pip) -> Whole;
auto tinted(STRING::Hot stem, Whole tier) -> String;
void tinted(SHELL::Session &session);

void dressed(
  GUI::Handle page, const Vector<Pip> &wanted, const Vector<Pip> &shown);

struct Box {
  GUI::Position at;
  GUI::Extent size;
  auto operator==(const Box &) const -> Bool = default;
};
auto boxed(const Pip &pip) -> Box;

void gathered(const LANE::Clip &span, Whole track, Vector<Pip> &wanted);

auto ghosts() -> Vector<Pip>;
void ghosts(GUI::Handle page);
void ghosts(SHELL::Session &session);

namespace HEADS {

constexpr Whole RESTING = 0, CHOSEN = 1, AFLOAT = 2;

constexpr Whole SOFT = 0, EVEN = 1, LOUD = 2;

struct Worn {
  Whole pitch = NONE, ink = NONE, notation = NONE, tier = NONE, loud = NONE;
  Integer place = 0;
  Float side = 0.0f;
  Flag flipped = false;
  Whole sign = NONE;     // the pitched staff: the accidental before the head
  Flag hollow = false;   // the pitched staff: a half note or longer
  Flag pitched = false;  // drawn on the pitched staff, not the drum's
  auto operator==(const Worn &) const -> Bool = default;
};

struct Child {
  STRING::Hot name, kind;
  GUI::Position at;
  GUI::Extent size;
  Float letter = 0.0f;
};
extern const Child HEAD, MARK, LEDGER, ACCENT, GHOST, SIGN;

auto named(Whole row, const Child &child) -> String;

void stand(GUI::Handle page, Whole row);
void drop(GUI::Handle page, Whole row);
void dress(GUI::Handle page, Whole row, const Worn &now);

auto side(const Vector<Pip> &pips) -> Float;
void sized(GUI::Handle page, Whole row, Float side, Float aside);
void ruled(GUI::Handle page, Whole row, Float wide, Float aside);

auto banded(const Pip &pip) -> Whole;
void stressed(GUI::Handle page, Whole row, const Worn &now, Float aside);

auto flipped(const Vector<Pip> &pips) -> Vector<Flag>;
auto shift(const Worn &now) -> Float;

void seat(GUI::Handle page, const Vector<Pip> &wanted);

}  // namespace HEADS

namespace SHED {
void heads();
void ghosts();
}  // namespace SHED

}  // namespace SOUND::VIEWS::ROLL

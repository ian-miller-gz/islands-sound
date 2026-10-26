// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL {

void loudness();
void loudness(SHELL::Session &session);

namespace SHED {
void loudness();
}  // namespace SHED

namespace LOUDNESS {

constexpr STRING::Hot STRIP = "piano.loudness";
constexpr STRING::Hot STEMS = "piano.loudness.stem";
constexpr Float SOFTEST = 1.0f / FULL;
constexpr Float LOUDEST = 1.0f;
constexpr Float WIDE = 4.0f;

struct Stem {
  Whole row = NONE, stock = NONE, index = NONE;
  Float across = 0.0f;
};

auto gathered() -> Vector<Stem>;

void seat(GUI::Handle page, const Vector<Stem> &stems);

void inset(GUI::Handle page, Flag open);

void bare(GUI::Handle page);

auto stems() -> const Vector<Stem> &;

auto deep() -> Float;

void stroke(GUI::Handle page, const Vector<Stem> &stems);

auto velocity(const Stem &stem) -> Float;

struct Touch {
  Whole stock = NONE, index = NONE;
  Float from = 0.0f, velocity = 0.0f;
};

struct Hold {
  Flag standing = false, relative = false;
  Float x = 0.0f, y = 0.0f;
  Vector<Touch> touched;
};

auto clamped(Float velocity) -> Float;
auto scored(Whole stock, Whole index) -> Float;

auto touch(Hold &hold, Whole stock, Whole index) -> Touch &;

void wrote(const Hold &hold);

}  // namespace LOUDNESS

}  // namespace SOUND::VIEWS::ROLL

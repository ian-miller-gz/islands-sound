// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/gui.hpp>

#include "../stock.hpp"

namespace SOUND::VIEWS::ENDS {

struct Line {
  Whole stock = NONE;
  Whole floor = 0, length = 0;
  Float west = 0.0f, room = 0.0f;
};

struct Bed {
  String board;
  Float deep = 0.0f, scale = 1.0f;
  Whole grain = 1;
};

struct Pool {
  Whole stood = 0;
  Vector<Float> worn;
};

constexpr STRING::Hot BED = "end";
constexpr STRING::Hot HEAD = "head";
constexpr STRING::Hot RUN = "run";
constexpr Float HEADED = 8.0f;

auto bedded(const Bed &bed) -> String;

auto headed(const Bed &bed) -> Float;

void build(GUI::Handle page, const Bed &bed, Whole count, Pool &pool);

void lay(
  GUI::Handle page, const Bed &bed, const Vector<Line> &lines, Pool &pool);

void bare(GUI::Handle page, const Bed &bed, Pool &pool);

auto named(const Bed &bed, Whole row) -> String;

}  // namespace SOUND::VIEWS::ENDS

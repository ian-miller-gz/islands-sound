// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../lane/lane.face.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr Whole TONAL = 0;
constexpr Whole PERCUSSION = 1;
constexpr STRING::Hot NOTATIONS[] = {"tonal", "percussion"};

constexpr STRING::Hot TONIC = "piano.bench.tonic";
constexpr STRING::Hot SCALE = "piano.bench.scale";
constexpr STRING::Hot MAP = "piano.bench.map";

namespace GET {

auto notation() -> Whole;

auto mapped() -> Vector<String>;

auto percussions() -> Vector<String>;

}  // namespace GET

auto faced() -> LANE::Face &;

void notate();
void notate(SHELL::Session &session);

void map();

}  // namespace SOUND::VIEWS::ROLL

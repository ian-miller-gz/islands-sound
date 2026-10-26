// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../lane/lane.face.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr Whole TONAL = 0;
constexpr Whole PERCUSSION = 1;
constexpr STRING::Hot NOTATIONS[] = {"tonal", "percussion"};

namespace PRESENTATION {
constexpr Whole GRID = 0, STAFF = 1;
}  // namespace PRESENTATION
constexpr STRING::Hot PRESENTATIONS[] = {"grid", "staff"};
constexpr STRING::Hot ABSENT = "none";

constexpr STRING::Hot TONIC = "piano.bench.tonic";
constexpr STRING::Hot SCALE = "piano.bench.scale";
constexpr STRING::Hot MAP = "piano.bench.map";

namespace GET {

auto notation() -> Whole;

auto presentation() -> Whole;

auto mapped() -> Vector<String>;

auto instruments() -> Vector<String>;

auto voices() -> Vector<AUDIO::PLUGIN::Voice>;

auto railed() -> Vector<Whole>;

}  // namespace GET

auto faced() -> LANE::Face &;

auto most() -> GUI::Extent;

void notate();
void notate(SHELL::Session &session);

void present();
void present(SHELL::Session &session);

void map();

}  // namespace SOUND::VIEWS::ROLL

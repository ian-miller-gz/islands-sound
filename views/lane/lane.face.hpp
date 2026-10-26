// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../onset.hpp"
#include "../../score.hpp"
#include "../views.hpp"

namespace SOUND::ARRANGEMENT::TRACK::LANE {

struct Clip;

}  // namespace SOUND::ARRANGEMENT::TRACK::LANE

namespace SOUND::VIEWS::LANE {

using Clip = SOUND::ARRANGEMENT::TRACK::LANE::Clip;

auto face() -> const View &;

void paged(Whole kind);

auto editable(Whole kind) -> Flag;

struct Mark {
  Whole track = NONE, lane = NONE, kind = NONE;
};

void mark(Whole track, Whole lane);

auto marked() -> Mark;

void marked(SHELL::Session &session);

auto marked(const FIELDS::Map &face) -> Flag;

auto edited(Whole kind) -> Mark;

constexpr Float UNSEEN = 0.0f;

struct Added {
  Whole tonic = 0;
  String scale;
  Whole pitch = 0;
};

struct Face {
  String track, lane;
  GUI::NGA::Pan pan;
  GUI::NGA::Zoom zoom = {UNSEEN};
  Whole number = NONE;
  Whole notation = 0;
  Whole presentation = 0;
  Vector<String> mapped;
  Vector<Added> added;
  Whole clip = NONE;
  Whole onsets = 0;
  Whole notes = 0;
  Whole loudness = 0;
  Float sense = ONSET::RESTING;
};

auto faced(const Mark &mark) -> Face &;

void faces(SHELL::Session &session);

void keep(FIELDS::Map &face);

void restore(const FIELDS::Map &face);

auto scope(Whole clip) -> Flag;

auto words() -> Vector<SHELL::Command>;

namespace GET {

auto seen() -> Flag;

auto scoped() -> const Clip *;

auto admitted(Whole at) -> Flag;

}  // namespace GET

}  // namespace SOUND::VIEWS::LANE

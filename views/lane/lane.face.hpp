// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../onset.hpp"
#include "../../score.hpp"
#include "../views.hpp"
#include "lane.mark.hpp"

namespace SOUND::VIEWS::LANE {

auto face() -> const View &;

void marked(SHELL::Session &session);

auto marked(const FIELDS::Map &face) -> Flag;

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

auto words() -> Vector<SHELL::Command>;

}  // namespace SOUND::VIEWS::LANE

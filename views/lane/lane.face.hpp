// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../score.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::LANE {

auto face() -> const View &;

void paged(Whole kind);

struct Mark {
  Whole track = NONE, lane = NONE, kind = NONE;
};

void mark(Whole track, Whole lane);

auto marked() -> Mark;

void marked(SHELL::Session &session);

auto edited(Whole kind) -> Mark;

constexpr Float UNSEEN = 0.0f;

struct Face {
  String track, lane;
  GUI::NGA::Pan pan;
  GUI::NGA::Zoom zoom = {UNSEEN};
  Whole number = NONE;
  Whole notation = 0;
  Vector<String> mapped;
};

auto faced(const Mark &mark) -> Face &;

void faces(SHELL::Session &session);

void keep(FIELDS::Map &face);

void restore(const FIELDS::Map &face);

}  // namespace SOUND::VIEWS::LANE

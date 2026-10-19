// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <deque>

#include "../views.hpp"
#include "lane.face.hpp"

namespace SOUND::VIEWS::LANE {

auto faced() -> const View *;

auto faced(Whole kind) -> const View *;

auto field() -> String;

auto field(Whole kind) -> String;

auto windowed() -> Float;

auto map() -> String;
auto slide() -> String;

constexpr Float RATIO = 0.1f;

void minimap();

void minimap(SHELL::Session &session);

void slider();

auto stored() -> Mark &;

auto records() -> std::deque<Face> &;

void keep(FIELDS::Map &face, Whole n, Face &one);

void restore(const FIELDS::Map &face, Whole n, Face &one);

void swap(Whole kind);

void record();

void forget();

namespace SHED {

void minimap();

}  // namespace SHED

}  // namespace SOUND::VIEWS::LANE

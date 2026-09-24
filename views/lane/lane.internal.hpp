// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::LANE {

auto faced() -> const View *;

auto field() -> String;

auto windowed() -> Float;

auto map() -> String;
auto slide() -> String;

constexpr Float RATIO = 0.1f;

void minimap();

void minimap(SHELL::Session &session);

void slider();

namespace SHED {

void minimap();

}  // namespace SHED

}  // namespace SOUND::VIEWS::LANE

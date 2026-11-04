// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../lane/lane.face.hpp"
#include "../views.hpp"
#include "piano.rows.hpp"

namespace SOUND::VIEWS::ROLL {

auto face() -> const View &;

auto words() -> Vector<SHELL::Command>;

}  // namespace SOUND::VIEWS::ROLL

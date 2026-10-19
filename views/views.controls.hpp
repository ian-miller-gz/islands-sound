// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS {

void wheeled(const View &unit, const GUI::NGA::Wheel &turn);

auto zoom(const View &unit, Float across, Float down) -> Flag;
auto pan(const View &unit, Float x, Float y) -> Flag;

namespace GET {

auto face(const View &unit) -> const View &;

auto board(const View &unit) -> String;

}  // namespace GET

}  // namespace SOUND::VIEWS

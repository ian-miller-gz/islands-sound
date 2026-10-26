// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

void opened(GUI::Handle page);

auto opened(GUI::Handle page, Whole row) -> Flag;

namespace BOX {

auto labels(Whole row, Float across) -> Vector<String>;

void take(Whole row, Whole taken);

}  // namespace BOX

}  // namespace SOUND::VIEWS::ARRANGEMENT

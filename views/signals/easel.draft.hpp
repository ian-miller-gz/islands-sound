// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "signals.plotted.hpp"

namespace SOUND::VIEWS::SIGNALS::DRAFT {

auto points() -> Vector<Point> &;

auto spanning() -> Flag &;

void lay(Integer across, Float value, Whole shape);

auto standing(Whole at) -> Whole;

}  // namespace SOUND::VIEWS::SIGNALS::DRAFT

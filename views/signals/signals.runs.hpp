// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "signals.plotted.hpp"

namespace SOUND::VIEWS::SIGNALS {

struct Run {
  Whole kind = NONE;
  Flag continuous = false;
  auto (*row)(const Stock &held, Whole index) -> Row = nullptr;
  auto (*covered)(const Stock &held, Whole at) -> Whole = nullptr;
  auto (*write)(Whole stock, const Row &laid) -> Flag = nullptr;
  auto (*between)(const Stock &held, Whole opens, Whole closes)
    -> Vector<Whole> = nullptr;
};

auto continuous() -> Flag;

namespace RUNS {

auto paged() -> const Run *;

}  // namespace RUNS

}  // namespace SOUND::VIEWS::SIGNALS

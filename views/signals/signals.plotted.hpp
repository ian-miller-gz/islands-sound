// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace SOUND::VIEWS::SIGNALS {

auto addressed() -> Address;

auto crossed(Float quantity) -> Float;

auto plotted(const Stock &stock, const ARRANGEMENT::TRACK::LANE::Clip &span)
  -> Vector<Point>;

void plotted(SHELL::Session &session);

auto claimed(Whole at) -> Aim;

namespace CURVE {

auto row(const Stock &stock, Whole index) -> Row;
auto covered(const Stock &stock, Whole at) -> Whole;
auto write(Whole stock, const Row &laid) -> Flag;
auto between(const Stock &stock, Whole opens, Whole closes) -> Vector<Whole>;

}  // namespace CURVE

}  // namespace SOUND::VIEWS::SIGNALS

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND::VIEWS::GRID {

using Reader = Whole (*)();
auto state(Reader reader) -> Flag;

}  // namespace SOUND::VIEWS::GRID

namespace SOUND::VIEWS::GET {

auto grid() -> Whole;

}  // namespace SOUND::VIEWS::GET

namespace SOUND::VIEWS {

auto quantized(Whole track, Whole lane, Whole placement, Float sense) -> Whole;

}  // namespace SOUND::VIEWS

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "views.quantize.hpp"

namespace {
using namespace SOUND;

VIEWS::GRID::Reader reader = nullptr;

}  // namespace

auto SOUND::VIEWS::GRID::state(Reader reader) -> Flag {
  ::reader = reader;
  return true;
}

auto SOUND::VIEWS::GET::grid() -> Whole {
  return ::reader == nullptr ? 0 : ::reader();
}

auto SOUND::VIEWS::quantized(
  Whole track, Whole lane, Whole placement, Float sense) -> Whole {
  return COMMANDS::quantized(track, lane, placement, GET::grid(), sense);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/carry.hpp>

#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL::HANDS {

struct Shift {
  Integer at = 0;
  Integer pitch = 0;
};

auto settled(Whole pulses) -> Whole;

auto shifted(const Pip &pip, const GUI::SAC::CARRY::Drop &landing) -> Shift;

auto moved(Whole at, Integer by) -> Whole;

auto raised(Whole pitch, Integer by, Whole at) -> Whole;

}  // namespace SOUND::VIEWS::ROLL::HANDS

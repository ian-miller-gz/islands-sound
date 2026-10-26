// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/carry.hpp>

#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL::HANDS {

struct Shift {
  Integer at = 0;
  Integer rows = 0;
};

auto settled(Whole pulses) -> Whole;

auto shifted(const Pip &pip, const GUI::SAC::CARRY::Drop &landing) -> Shift;

auto moved(Whole at, Integer by) -> Whole;

auto raised(const Pip &pip, Integer by, Whole at) -> Whole;

auto handle(Whole row) -> String;

auto resting(Whole length) -> Float;

void grip(GUI::Handle page, const Vector<Pip> &drawn);

}  // namespace SOUND::VIEWS::ROLL::HANDS

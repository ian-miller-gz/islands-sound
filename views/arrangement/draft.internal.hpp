// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "arrangement.internal.hpp"

namespace SOUND::VIEWS::ARRANGEMENT::DRAFT {

struct Run {
  Whole opens = 0, closes = 0;
};

auto pulled(const GUI::SAC::STROKE::Gesture &gesture, Whole step, Flag settle)
  -> Run;

auto pencilled(const GUI::SAC::STROKE::Gesture &gesture) -> Flag;

auto seated(const GUI::SAC::STROKE::Gesture &gesture) -> Row;

void landed(
  const GUI::SAC::LADDER::Rung &step, const GUI::SAC::STROKE::Gesture &gesture);

}  // namespace SOUND::VIEWS::ARRANGEMENT::DRAFT

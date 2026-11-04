// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../arrangement.hpp"
#include "../lane/lane.mark.hpp"

namespace SOUND::VIEWS::WAVE {

using Clip = ARRANGEMENT::TRACK::LANE::Clip;

auto laned() -> LANE::Mark;
auto placed() -> Vector<Clip>;

auto chosen() -> Whole;

void retouch(Whole row, const Clip &was, const Clip &now, STRING::Hot act);

}  // namespace SOUND::VIEWS::WAVE

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/ladder.hpp>

#include "../transport.hpp"

namespace SOUND::VIEWS {

using Ladder = Vector<GUI::SAC::LADDER::Rung> (*)(Whole bar);

auto dressed(
  const Vector<TRANSPORT::Section> &sections, Ladder ladder, Float scale,
  Float crowd, Float pan, Float seen) -> Vector<GUI::SAC::LADDER::Mark>;

auto dressed(
  const Vector<TRANSPORT::Section> &sections,
  const GUI::SAC::LADDER::Rung &grain, Whole factor, Float scale, Float crowd,
  Float pan, Float seen) -> Vector<GUI::SAC::LADDER::Mark>;

auto numbered(const Vector<TRANSPORT::Section> &sections, Integer at) -> String;

auto standing(const Vector<TRANSPORT::Section> &sections, Whole at)
  -> TRANSPORT::Section;

auto spanned(const Vector<GUI::SAC::LADDER::Mark> &marks, Whole at, Whole span)
  -> Whole;

}  // namespace SOUND::VIEWS

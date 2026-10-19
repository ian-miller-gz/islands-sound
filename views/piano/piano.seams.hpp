// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL::SEAMS {

struct Seam {
  Whole west = NONE, east = NONE;
  Whole at = 0;
  Flag flush = false;
};

void crossed(
  GUI::Handle page, const String &bed, const Vector<Seam> &seams, Whole &stood);

}  // namespace SOUND::VIEWS::ROLL::SEAMS

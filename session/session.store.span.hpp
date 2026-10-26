// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <sstream>

#include "../arrangement.hpp"

namespace SOUND::SESSION {

auto spanned(const ARRANGEMENT::TRACK::LANE::Clip &span) -> String;
auto spanned(std::istringstream &line) -> ARRANGEMENT::TRACK::LANE::Clip;

auto tuned(const ARRANGEMENT::TRACK::LANE::Clip &span) -> String;
void tune(std::istringstream &line, ARRANGEMENT::TRACK::LANE::Clip &span);
void tune(std::istringstream &line, ARRANGEMENT::TRACK::Lane &lane);
void hung(
  const String &tag, std::istringstream &line, ARRANGEMENT::TRACK::Lane &lane);

}  // namespace SOUND::SESSION

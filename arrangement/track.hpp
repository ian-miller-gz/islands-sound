// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"
#include "track/lane.hpp"

namespace SOUND::ARRANGEMENT {

struct Track {
  String name;
  Vector<TRACK::Lane> lanes;
  String root;
  Flag bus = false;
};

}  // namespace SOUND::ARRANGEMENT

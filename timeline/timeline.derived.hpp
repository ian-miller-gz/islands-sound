// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"
#include "../peaks.hpp"
#include "../stock.hpp"

namespace SOUND::TIMELINE {

struct Derived {
  const Take *take = nullptr;
  const PEAKS::Level *level = nullptr;
};

auto derived(Whole stock, const ARRANGEMENT::TRACK::LANE::Clip &clip)
  -> Derived;

auto placed(
  const ARRANGEMENT::TRACK::LANE::Clip &clip,
  const Vector<Whole> &frames) -> Vector<Whole>;

}  // namespace SOUND::TIMELINE

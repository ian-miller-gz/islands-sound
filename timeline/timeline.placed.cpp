// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "timeline.derived.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;

auto opening(const Clip &clip) -> Whole {
  if (clip.ratio == Clip{}.ratio) return clip.from;
  return Whole(std::lround(std::max(Float(clip.from) * clip.ratio, 0.0f)));
}

}  // namespace

auto SOUND::TIMELINE::placed(const Clip &clip, const Vector<Whole> &frames)
  -> Vector<Whole> {
  const Derived source = derived(clip.stock, clip);
  if (source.take == nullptr || source.take->lanes.empty()) return {};
  const Whole carried = source.take->lanes[0].size();
  if (carried == 0) return {};
  const Whole from = ::opening(clip);
  Vector<Whole> laid;
  for (Whole frame : frames)
    for (Whole at = opened(frame, from, carried); at < clip.frames;
         at += carried)
      laid.push_back(clip.at + at);
  std::ranges::sort(laid);
  return laid;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

auto covering(const ARRANGEMENT::TRACK::LANE::Clip &span, Whole at) -> Flag {
  return span.at <= at && at < span.at + span.frames;
}

}  // namespace

auto SOUND::TIMELINE::govern(
  const Inventory &pool, const Address &address, Whole at) -> Governed {
  Governed governing;
  Whole begun = 0;
  for (const ARRANGEMENT::Track &track : held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes) {
      if (lane.kind != KIND::LOGIC) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips) {
        if (span.stock >= pool.stocks.size() || span.frames == 0) continue;
        if (!::covering(span, at) || (governing.stated && span.at < begun))
          continue;
        const Curve &curve = pool.stocks[span.stock].curve;
        if (
          curve.points.empty() ||
          curve.address.parameter != address.parameter ||
          curve.address.node != address.node)
          continue;
        governing = {
          .stated = true, .value = valued(curve, span.from + (at - span.at))};
        begun = span.at;
      }
    }
  return governing;
}

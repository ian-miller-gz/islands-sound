// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

struct Key {
  Whole opens = 0, closes = 0, kind = NONE, number = NONE, counted = NONE;
  Vector<Whole> clips;
  auto operator==(const Key &other) const -> Flag = default;
};

Key key;
Vector<VIEWS::SIGNALS::Point> kept;

auto keyed() -> Key {
  const VIEWS::SIGNALS::Span lit = VIEWS::SIGNALS::seen();
  Key now = {
    lit.opens, lit.closes, VIEWS::SIGNALS::paged(), VIEWS::SIGNALS::number(),
    INVENTORY::counted()};
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      if (lane.kind == now.kind)
        for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
          now.clips.insert(
            now.clips.end(), {span.stock, span.at, span.from, span.frames});
  return now;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::points() -> Vector<Point> {
  ::Key now = ::keyed();
  if (now == ::key) return ::kept;
  ::key = std::move(now);
  ::kept = gather({NONE, ::key.opens, ::key.closes});
  return ::kept;
}

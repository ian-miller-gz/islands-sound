// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "piano.internal.hpp"

auto SOUND::VIEWS::ROLL::laned(Whole track) -> Whole {
  const LANE::Mark mark = LANE::edited(KIND::NOTES);
  return mark.track == track ? mark.lane : NONE;
}

auto SOUND::VIEWS::ROLL::spans() -> Vector<Span> {
  const LANE::Mark mark = LANE::edited(KIND::NOTES);
  const Inventory &pool = INVENTORY::held();
  Vector<Span> lit;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(mark.track, mark.lane))
    lit.push_back(
      {span.stock, opened(span.at), opened(span.at + span.frames),
       span.stock < pool.stocks.size() ? pool.stocks[span.stock].name
                                       : String("?")});
  return lit;
}

auto SOUND::VIEWS::ROLL::aimed(Whole track, Whole at) -> Aim {
  const ARRANGEMENT::TRACK::LANE::Clip span =
    TIMELINE::covered(track, laned(track), at);
  if (span.stock == NONE) return {};
  return {span.stock, span.from, opened(span.at)};
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../history.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {

Whole answered = 0;

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::rowed(Whole track, Whole lane, Whole placement)
  -> Whole {
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  Whole row = 0;
  for (Whole one = 0; one < tracks.size(); ++one)
    for (Whole at = 0; at < tracks[one].lanes.size(); ++at) {
      if (one == track && at == lane) return row + placement;
      row += tracks[one].lanes[at].clips.size();
    }
  return NONE;
}

void SOUND::VIEWS::ARRANGEMENT::walk() {
  const Walk &walked = HISTORY::walked();
  if (walked.mark == ::answered) return;
  ::answered = walked.mark;
  if (!walked.placing) return;
  Vector<Whole> rows;
  for (const Placement &one : walked.placements)
    rows.push_back(rowed(one.track, one.lane, one.placement));
  chosen(rows);
}

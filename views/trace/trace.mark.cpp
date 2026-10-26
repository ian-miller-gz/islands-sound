// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arranger/arranger.face.hpp"
#include "../lane/lane.face.hpp"
#include "trace.internal.hpp"

namespace LANE = SOUND::VIEWS::LANE;

void SOUND::VIEWS::TRACE::marked(GUI::Handle page) {
  const Whole cursor = GUI::GET::cursor(page, LANES);
  if (GUI::GET::selected(page, LANES)) LANE::mark(attended(), cursor);
  if (GUI::GET::activated(page, LANES)) ARRANGER::edit(attended(), cursor);
  const LANE::Mark mark = LANE::marked();
  if (mark.lane != NONE && mark.lane != cursor)
    GUI::set(page, LANES, GUI::Cursor{mark.lane});
}

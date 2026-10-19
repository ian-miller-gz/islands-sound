// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../lane/lane.face.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;
namespace LANE = SOUND::VIEWS::LANE;

auto taken(GUI::Handle page) -> Flag {
  const Flag selected = GUI::GET::selected(page, TRACE::LANES);
  return GUI::GET::activated(page, TRACE::LANES) || selected;
}

}  // namespace

void SOUND::VIEWS::TRACE::marked(GUI::Handle page) {
  const Whole cursor = GUI::GET::cursor(page, LANES);
  if (::taken(page)) LANE::mark(attended(), cursor);
  const LANE::Mark mark = LANE::marked();
  if (mark.lane != NONE && mark.lane != cursor)
    GUI::set(page, LANES, GUI::Cursor{mark.lane});
}

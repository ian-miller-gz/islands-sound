// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../arranger/arranger.face.hpp"
#include "../lane/lane.face.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot PLACED = "at";
constexpr STRING::Hot MARK = "selected";
constexpr STRING::Hot SPAN = "{} · {}";

Whole pending = NONE;

void pressed(const TRACE::Clipped &row) {
  if (row.scoped) {
    LANE::scope(NONE);
    return;
  }
  const LANE::Mark mark = LANE::marked();
  VIEWS::ARRANGER::edit(mark.track, mark.lane);
  ::pending = row.clip;
}

void rowed(GUI::Handle page, const Vector<TRACE::Clipped> &rows) {
  const Whole first = GUI::GET::first(page, TRACE::CLIPS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", TRACE::CLIPS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const TRACE::Clipped &one = rows[first + row];
    if (GUI::GET::clicked(page, cell.c_str())) ::pressed(one);
    GUI::set(page, (cell + "." + ::NAMED).c_str(), GUI::Text{one.name});
    GUI::set(
      page, (cell + "." + ::PLACED).c_str(),
      GUI::Text{std::format(::SPAN, one.place, one.bars)});
  }
}

void marked(GUI::Handle page, const Vector<TRACE::Clipped> &rows) {
  for (Whole row = 0; row < rows.size(); ++row) {
    if (!rows[row].scoped) continue;
    GUI::set(page, TRACE::CLIPS, GUI::Cursor{row});
    GUI::set(page, TRACE::CLIPS, GUI::Mark{::MARK});
    return;
  }
  GUI::set(page, TRACE::CLIPS, GUI::Mark{});
}

}  // namespace

void SOUND::VIEWS::TRACE::clips() {
  const GUI::Handle page = document();
  if (::pending != NONE) LANE::scope(::pending);
  ::pending = NONE;
  const Vector<Clipped> rows = clipped();
  GUI::set(page, CLIPS, GUI::Rows{rows.size()});
  GUI::set(page, EMPTY, GUI::Visibility{rows.empty()});
  ::rowed(page, rows);
  ::marked(page, clipped());
}

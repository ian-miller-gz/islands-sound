// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <island/gui/tip.hpp>

#include "../../kind.hpp"
#include "../arranger/arranger.face.hpp"
#include "../lane/lane.mark.hpp"
#include "../views.letters.hpp"
#include "../views.split.regions.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot KINDED = "kind";
constexpr STRING::Hot PLACED = "at";
constexpr STRING::Hot TINTED = "tint";
constexpr STRING::Hot PLATE = "plate";
constexpr STRING::Hot MARK = "selected";
constexpr STRING::Hot SPAN = "{} · {}";

Whole pending = NONE;

void pressed(const TRACE::Clipped &row) {
  if (row.scoped) {
    const VIEWS::SPLIT::Turn turn(VIEWS::SPLIT::turned());
    LANE::scope(NONE);
    return;
  }
  const Whole track = TRACE::attended();
  if (track == NONE || !VIEWS::ARRANGER::edit(track, row.lane)) return;
  ::pending = row.clip;
}

void named(GUI::Handle page, const String &id, const String &whole) {
  const String shown = VIEWS::LETTERS::cropped(
    whole, GUI::GET::measured(page, id.c_str()).w,
    GUI::GET::size(page, id.c_str()));
  GUI::set(page, id.c_str(), GUI::Text{shown});
  GUI::TIP::note(page, id.c_str(), shown == whole ? String() : whole);
}

void rowed(GUI::Handle page, const Vector<TRACE::Clipped> &rows) {
  const Whole first = GUI::GET::first(page, TRACE::CLIPS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", TRACE::CLIPS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const TRACE::Clipped &one = rows[first + row];
    if (GUI::GET::clicked(page, cell.c_str())) ::pressed(one);
    ::named(page, cell + "." + ::NAMED, one.name);
    GUI::set(
      page, (cell + "." + ::KINDED).c_str(),
      GUI::Text{String(KIND::spoken(one.kind))});
    GUI::set(
      page, (cell + "." + ::PLACED).c_str(),
      GUI::Text{std::format(::SPAN, one.place, one.bars)});
    const String plate = ::PLATE + String(KIND::spoken(one.kind));
    GUI::set(page, (cell + "." + ::TINTED).c_str(), GUI::Style{plate.c_str()});
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
  if (::pending != NONE) {
    const SPLIT::Turn turn(SPLIT::turned());
    LANE::scope(::pending);
  }
  ::pending = NONE;
  const Vector<Clipped> rows = clipped();
  GUI::set(page, CLIPS, GUI::Rows{rows.size()});
  GUI::set(page, EMPTY, GUI::Visibility{rows.empty()});
  ::rowed(page, rows);
  ::marked(page, clipped());
}

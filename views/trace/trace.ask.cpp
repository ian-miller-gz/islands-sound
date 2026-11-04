// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "../arranger/arranger.face.hpp"
#include "../views.menu.acts.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;
namespace ARRANGER = SOUND::VIEWS::ARRANGER;

constexpr STRING::Hot EDITS = "Edit";
constexpr STRING::Hot VELOCITY = "Velocity";

auto shown(Whole row) -> Flag {
  const Whole track = TRACE::attended();
  if (track == NONE) return false;
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  return row < lanes.size() && ARRANGER::editable(lanes[row].kind);
}

auto noted(Whole row) -> Flag {
  const Whole track = TRACE::attended();
  if (!::shown(row)) return false;
  return TIMELINE::held().tracks[track].lanes[row].kind == KIND::NOTES;
}

void edit(Whole row) { ARRANGER::edit(TRACE::attended(), row); }

void velocity(Whole row) { ARRANGER::velocity(TRACE::attended(), row); }

const Vector<VIEWS::MENU::Act> ACTS = {
  {::EDITS, ::edit, ::shown}, {::VELOCITY, ::velocity, ::noted}};

}  // namespace

void SOUND::VIEWS::TRACE::asked(GUI::Handle page) {
  VIEWS::MENU::offer(page, LANES, ::ACTS);
}

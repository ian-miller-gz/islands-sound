// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/stroke.hpp>

#include "../../timeline.hpp"
#include "arrangement.internal.hpp"
#include "tracks.internal.hpp"

namespace {
using namespace SOUND;

auto chose(const GUI::SAC::STROKE::Gesture &gesture) -> Whole {
  if (!gesture.closed || gesture.dragged) return NONE;
  const Whole track = VIEWS::ARRANGEMENT::ribboned(gesture.from.at.y);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::ribboned(Float down) -> Whole {
  const Vector<Ribbon> runs = ribbons();
  for (Whole track = 0; track < runs.size(); ++track)
    if (runs[track].top <= down && down < runs[track].bottom) return track;
  return NONE;
}

void SOUND::VIEWS::ARRANGEMENT::choose() {
  const GUI::Handle page = VIEWS::document();
  const Whole track = ::chose(GUI::SAC::STROKE::GET::gesture(page));
  if (track == NONE) return;
  GUI::set(page, VIEWS::TRACKS::ROWS, GUI::Cursor{track});
}

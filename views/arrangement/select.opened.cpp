// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <island/gui/seat.hpp>

#include "../lane/lane.face.hpp"
#include "arrangement.face.hpp"
#include "arrangement.internal.hpp"
#include "arrangement.opened.hpp"
#include "tracks.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

Flag wanted = false;

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::opened(GUI::Handle page) {
  const String id = board();
  if (!GUI::GET::activated(page, id.c_str())) return;
  opened(page, GUI::SAC::SEAT::selected(page, id.c_str()));
}

auto SOUND::VIEWS::ARRANGEMENT::opened(GUI::Handle page, Whole row) -> Flag {
  if (row >= boxes().size()) return false;
  const Box box = boxes()[row];
  if (!LANE::editable(box.kind)) return false;
  GUI::set(page, TRACKS::ROWS, GUI::Cursor{box.track});
  LANE::mark(box.track, box.lane);
  LANE::faced(LANE::marked()).clip = box.placement;
  ::wanted = true;
  return true;
}

auto SOUND::VIEWS::ARRANGEMENT::GET::opened() -> Flag {
  return std::exchange(::wanted, false);
}

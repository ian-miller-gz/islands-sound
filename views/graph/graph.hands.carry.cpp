// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "../../commands.hpp"
#include "../../graph.hpp"
#include "../../history.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

Flag carrying = false;

}  // namespace

void SOUND::VIEWS::WIRED::carried(GUI::Handle page) {
  Flag moved = false;
  for (Whole row = 0; row < GRAPH::held().nodes.size(); ++row) {
    const String box = GUI::SAC::SEAT::named(board().c_str(), row);
    if (!GUI::GET::moved(page, box.c_str())) continue;
    if (!::carrying) {
      HISTORY::begin();
      ::carrying = true;
    }
    moved = true;
    const GUI::Position at = GUI::GET::position(page, box.c_str());
    COMMANDS::homed(GRAPH::held().nodes[row].name, steering(), at.x, at.y);
  }
  if (moved || !::carrying) return;
  HISTORY::end();
  ::carrying = false;
}

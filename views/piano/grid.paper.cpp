// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "piano.grid.hpp"
#include "piano.notation.hpp"
#include "piano.paper.hpp"
#include "piano.staff.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr STRING::Hot INK = "staffstem";

Flag hidden = false;

auto shown(GUI::Handle page, const String &id, Float pan) -> Flag {
  const Float x = GUI::GET::position(page, id.c_str()).x;
  return (x - pan) * ROLL::scaled() >= ROLL::margin();
}

}  // namespace

auto SOUND::VIEWS::ROLL::papered() -> Flag {
  return GET::staffed() && GET::presentation() == PRESENTATION::STAFF;
}

void SOUND::VIEWS::ROLL::PAPER::grid(
  const Vector<GUI::SAC::LADDER::Mark> &marks) {
  const GUI::Handle page = document();
  const Flag laid = papered();
  if (!laid && !::hidden) return;
  const GUI::Extent bar = {
    WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::BAR}), STAFF::spanned().h};
  const Float pan = GUI::NGA::GET::pan(page, board().c_str()).x;
  for (Whole at = 0; at < marks.size(); ++at) {
    const String id = GRID::named(at);
    const Flag standing = marks[at].opens && ::shown(page, id, pan);
    GUI::set(page, id.c_str(), GUI::Visibility{!laid || standing});
    if (!laid || !standing) continue;
    GUI::set(page, id.c_str(), bar);
    GUI::set(page, id.c_str(), GUI::Style{::INK});
  }
  ::hidden = laid;
}

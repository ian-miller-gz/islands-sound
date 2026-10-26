// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "piano.grid.hpp"
#include "piano.notation.hpp"
#include "piano.paper.hpp"

void SOUND::VIEWS::ROLL::PAPER::state(SHELL::Session &session) {
  if (GET::notation() != PERCUSSION) return;
  const Vector<GUI::SAC::LADDER::Mark> marks = GRID::dressed();
  Whole bars = 0, beats = 0;
  for (Whole at = 0; at < GRID::lit() && at < marks.size(); ++at)
    if (GUI::GET::visibility(document(), GRID::named(at).c_str()))
      ++(marks[at].opens ? bars : beats);
  session.print(
    std::format("paper bars {} beats {} wash {}", bars, beats, washed()));
}

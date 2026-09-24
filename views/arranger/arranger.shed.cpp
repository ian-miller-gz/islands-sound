// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "arranger.internal.hpp"

void SOUND::VIEWS::ARRANGER::shed() {
  VIEWS::shed(ARRANGEMENT::face());
  VIEWS::shed(LANE::face());
}

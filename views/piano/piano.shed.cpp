// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.loudness.hpp"
#include "piano.paper.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

void SOUND::VIEWS::ROLL::shed() {
  SHED::hands();
  SHED::heads();
  SHED::ghosts();
  SHED::plates();
  SHED::keys();
  SHED::grid();
  SHED::head();
  SHED::rail();
  SHED::plus();
  SHED::range();
  SHED::cycle();
  SHED::changes();
  SHED::ruler();
  SHED::seams();
  SHED::ends();
  SHED::loudness();
  STAFF::SHED::clef();
  STAFF::SHED::drawn();
  STAFF::SHED::stemmed();
}

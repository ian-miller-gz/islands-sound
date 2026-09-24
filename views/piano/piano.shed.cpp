// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.internal.hpp"

void SOUND::VIEWS::ROLL::shed() {
  SHED::hands();
  SHED::plates();
  SHED::keys();
  SHED::grid();
  SHED::head();
  SHED::rail();
  SHED::range();
  SHED::cycle();
  SHED::changes();
  SHED::ruler();
  SHED::seams();
}

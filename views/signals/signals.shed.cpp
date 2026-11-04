// SPDX-License-Identifier: AGPL-3.0-or-later
#include "signals.overridden.hpp"

void SOUND::VIEWS::SIGNALS::shed() {
  SHED::plates();
  SHED::segments();
  SHED::grid();
  SHED::head();
  SHED::rail();
  SHED::range();
  SHED::ends();
  SHED::ruler();
  SHED::restore();
}

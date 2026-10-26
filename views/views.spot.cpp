// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

auto SOUND::VIEWS::spot() -> GUI::Position {
  return GUI::GET::spot(document());
}

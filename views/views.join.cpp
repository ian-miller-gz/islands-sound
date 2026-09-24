// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "views.internal.hpp"

auto SOUND::VIEWS::joined(Whole track, Whole lane, const Vector<Whole> &rows)
  -> Whole {
  return COMMANDS::joined(track, lane, rows);
}

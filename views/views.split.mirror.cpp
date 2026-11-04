// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.hpp"

auto SOUND::VIEWS::SPLIT::mirror(STRING::Hot name) -> Flag {
  return name != nullptr && name[0] != '\0';
}

auto SOUND::VIEWS::SPLIT::mirrored(Whole row) -> Flag {
  return row < roster().size() && !roster()[row].panel && !hosted(row);
}

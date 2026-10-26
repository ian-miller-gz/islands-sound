// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "views.aim.hpp"

auto SOUND::VIEWS::aimed(Whole stock, const String &node, Whole parameter)
  -> Flag {
  return COMMANDS::aimed(stock, {.node = node, .parameter = parameter});
}

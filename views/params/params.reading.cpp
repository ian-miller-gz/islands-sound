// SPDX-License-Identifier: AGPL-3.0-or-later
#include "params.internal.hpp"

auto SOUND::VIEWS::RACK::fitted(
  GUI::Handle page, const String &cell, const Row &row) -> String {
  return RIG::fitted(page, cell + "." + DIALLED, shown(row));
}

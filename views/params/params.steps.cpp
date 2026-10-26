// SPDX-License-Identifier: AGPL-3.0-or-later
#include "params.internal.hpp"

auto SOUND::VIEWS::RACK::listed(const Row &row) -> Flag {
  return RIG::listed(described(row));
}

void SOUND::VIEWS::RACK::door(
  GUI::Handle page, const String &cell, const Row &row) {
  if (head(row)) return;
  const String id = cell + "." + STEPPED;
  GUI::set(page, id.c_str(), GUI::Visibility{listed(row)});
  if (GUI::GET::clicked(page, id.c_str())) return raise(page, id, row);
  seated(page, id, row);
}

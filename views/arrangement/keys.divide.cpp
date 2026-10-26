// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../commands.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"
#include "keys.internal.hpp"

void SOUND::VIEWS::ARRANGEMENT::divide(GUI::Handle) {
  const Whole at = TRANSPORT::marker().position;
  const Vector<Whole> rows = chosen();
  for (Whole back = rows.size(); back > 0; --back) {
    const Box &box = boxes()[rows[back - 1]];
    COMMANDS::split(box.track, box.lane, box.placement, at);
  }
  chosen({});
}

void SOUND::VIEWS::ARRANGEMENT::rename(GUI::Handle page) {
  const Vector<Whole> rows = chosen();
  if (!rows.empty()) rename(page, rows.front());
}

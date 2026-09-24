// SPDX-License-Identifier: AGPL-3.0-or-later
#include "history.internal.hpp"

namespace {

SOUND::Walk walk;

}  // namespace

void SOUND::HISTORY::walked(const Edit &edit, Flag back) {
  ::walk = {
    .mark = ::walk.mark + 1,
    .placing = !edit.placements.empty() || !edit.gone.empty(),
    .writing = !edit.notes.empty()};
  const Flag changed = edit.verb == Edit::CHANGED;
  if (changed || back == (edit.verb == Edit::REMOVED))
    ::walk.placements = edit.placements;
  if (back && !changed)
    ::walk.placements.insert(
      ::walk.placements.end(), edit.gone.begin(), edit.gone.end());
  for (const Noted &one : edit.notes)
    if ((back ? one.was.at : one.note.at) != NONE) ::walk.notes.push_back(one);
}

auto SOUND::HISTORY::walked() -> const Walk & { return ::walk; }

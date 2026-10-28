// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../session.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ASK = "recovery.ask";
constexpr STRING::Hot TAKE = "recovery.take";
constexpr STRING::Hot DROP = "recovery.drop";

Flag offered = false;

void raised(GUI::Handle page, Flag on) {
  GUI::set(page, ::ASK, GUI::Visibility{on});
}

}  // namespace

// The recovery plate (order 394q): once a run, at its first frame, the
// store is asked for a recovery row under the session's name — a run
// that never closed cleanly left one — and the plate offers it. Recover
// takes it through Open's own path, so the ledger comes back and undo
// stands; Discard forgets it; Escape lowers the plate and leaves the row
// for the next run to offer again.
void SOUND::VIEWS::recovery() {
  const GUI::Handle page = document();
  if (!::offered) {
    ::offered = true;
    if (SESSION::recovered(SESSION::held().name)) ::raised(page, true);
  }
  if (!GUI::GET::visibility(page, ::ASK)) return;
  if (GUI::GET::clicked(page, ::TAKE)) {
    COMMANDS::recover(SESSION::held().name);
    ::raised(page, false);
    return;
  }
  if (GUI::GET::clicked(page, ::DROP)) {
    SESSION::forget(SESSION::held().name);
    ::raised(page, false);
    return;
  }
  if (VIEWS::escaped()) ::raised(page, false);
}

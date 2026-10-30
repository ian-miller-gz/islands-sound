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

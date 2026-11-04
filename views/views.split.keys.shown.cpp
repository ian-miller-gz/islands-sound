// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {

constexpr STRING::Hot LIST = "split.keys";

}  // namespace

void SOUND::VIEWS::SPLIT::HOLD::shown(Flag on) {
  const GUI::Handle page = sheet();
  if (GUI::GET::visibility(page, ::LIST) != on)
    GUI::set(page, ::LIST, GUI::Visibility{on});
}

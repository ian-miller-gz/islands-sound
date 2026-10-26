// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "ruler.changes.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ERASES = "Delete";
constexpr STRING::Hot WHOSE = "timing";

VIEWS::ARRANGEMENT::CHANGES::Mark asking;

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::CHANGES::asked(const Vector<Mark> &marks) {
  if (::asking.erase != nullptr) {
    const Whole row = MENU::taken(::WHOSE);
    if (row != NONE) ::asking.erase(::asking.at);
    if (row != NONE || !MENU::standing()) ::asking = {};
    return;
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(document());
  if (!ask.asked || !ask.board.empty()) return;
  for (const Mark &mark : marks) {
    if (ask.target != mark.id) continue;
    ::asking = mark;
    return MENU::raise({String(::ERASES)}, ask.x, ask.y, ::WHOSE);
  }
}

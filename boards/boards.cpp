// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "boards.hpp"

namespace {
Whole standing = 0;
}  // namespace

auto SOUND::BOARDS::cursor(GUI::Handle document, STRING::Hot list) -> Whole {
  if (GUI::GET::rows(document, list) == 0) return NONE;
  return GUI::GET::cursor(document, list);
}

void SOUND::BOARDS::place(
  GUI::Handle document, STRING::Hot parent, STRING::Hot kind, const String &id,
  GUI::Position at, GUI::Extent size) {
  GUI::NODES::create(document, parent, kind, id.c_str());
  ++::standing;
  GUI::set(document, id.c_str(), at);
  GUI::set(document, id.c_str(), size);
}

void SOUND::BOARDS::drop(GUI::Handle document, const String &id) {
  GUI::NODES::remove(document, id.c_str());
  --::standing;
}

void SOUND::BOARDS::sweep(
  GUI::Handle document, STRING::Hot board, Whole from, Whole until) {
  for (Whole row = from; row < until; ++row)
    drop(document, GUI::SAC::SEAT::named(board, row));
}

auto SOUND::BOARDS::seated() -> Whole { return ::standing; }

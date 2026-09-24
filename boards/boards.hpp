// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/gui.hpp>

#include "../graph.hpp"

namespace SOUND::BOARDS {

constexpr STRING::Hot PLATE = "slab";

auto cursor(GUI::Handle document, STRING::Hot list) -> Whole;

void place(
  GUI::Handle document, STRING::Hot parent, STRING::Hot kind, const String &id,
  GUI::Position at, GUI::Extent size);

void drop(GUI::Handle document, const String &id);

void sweep(GUI::Handle document, STRING::Hot board, Whole from, Whole until);

auto seated() -> Whole;

}  // namespace SOUND::BOARDS

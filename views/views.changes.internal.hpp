// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.changes.hpp"

namespace SOUND::VIEWS::CHANGES {

struct Seats {
  STRING::Hot stem, widget;
  Float wide, down, head;
  auto (*erase)(Whole at) -> Flag;
  auto (*move)(Whole from, Whole to) -> Flag;
  Whole stood = 0;
};

struct Laid {
  String strip;
  Seats tempos, metres, keys;
  Whole born = 0;
};

auto seated(const String &strip) -> Laid &;

auto mark(const String &strip, const Seats &seats, Whole at) -> String;

void build(GUI::Handle page, const String &strip, Seats &seats, Whole count);

}  // namespace SOUND::VIEWS::CHANGES

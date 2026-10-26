// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::MENU {

struct Act {
  STRING::Hot word = "";
  void (*act)(Whole row) = nullptr;
  auto (*shown)(Whole row) -> Flag = nullptr;
};

auto asked(GUI::Handle page, STRING::Hot list) -> Whole;

auto offer(GUI::Handle page, STRING::Hot list, const Vector<Act> &acts) -> Flag;

}  // namespace SOUND::VIEWS::MENU

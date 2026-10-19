// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr STRING::Hot CROSS = "×";
constexpr STRING::Hot DIAMOND = "◆";

struct Drum {
  Whole from = 0, to = 0;
  STRING::Hot head = "";
};

auto headed(Whole note) -> STRING::Hot;

}  // namespace SOUND::VIEWS::ROLL

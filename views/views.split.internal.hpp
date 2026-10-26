// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.split.hpp"

namespace SOUND::VIEWS::SPLIT {

struct Seat {
  Float foot = SHUT, depth = SHUT, bar = SHUT, least = SHUT, room = SHUT;
};

auto measured(GUI::Handle page) -> Seat;

void rooted(GUI::Handle page, const Seat &seat, Float tall);

}  // namespace SOUND::VIEWS::SPLIT

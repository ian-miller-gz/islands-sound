// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::SIGNALS {

auto face() -> const View &;

auto paged() -> Whole;
void paged(Whole kind);

auto number() -> Whole;

void number(Whole paged);

}  // namespace SOUND::VIEWS::SIGNALS

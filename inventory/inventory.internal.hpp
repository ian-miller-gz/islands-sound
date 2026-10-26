// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "inventory.doors.hpp"

namespace SOUND::INVENTORY {

auto standing() -> Inventory &;

void stir();
void stir(Whole stock);

}  // namespace SOUND::INVENTORY

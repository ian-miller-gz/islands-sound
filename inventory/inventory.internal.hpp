// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "inventory.doors.hpp"
#include "inventory.riff.hpp"

namespace SOUND::INVENTORY {

auto standing() -> Inventory &;

constexpr STRING::Hot UNREADABLE = "unreadable";

auto refused(const Vector<Byte> &bytes, RIFF::Shape &shape) -> STRING::Hot;

void stir();
void stir(Whole stock);

}  // namespace SOUND::INVENTORY

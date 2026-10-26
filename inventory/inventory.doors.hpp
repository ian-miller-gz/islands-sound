// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "inventory.hpp"

namespace SOUND::INVENTORY {

auto held() -> const Inventory &;

auto counted() -> Whole;

auto touched() -> const Touched &;

void settled();

auto length(const Stock &stock) -> Whole;

void adopt(const Inventory &pool);

auto refuses(Whole kind) -> STRING::Hot;

auto add(const String &name, Whole kind) -> Whole;

auto score(Whole stock, const Score &content) -> Flag;

auto content(Whole stock, const Stock &clip) -> Flag;

auto take(Whole stock, const String &path) -> STRING::Hot;

auto aim(Whole stock, const Address &address) -> Flag;

auto plot(Whole stock, const Point &point) -> Flag;

auto turn(Whole stock, const Turn &move) -> Flag;
auto choose(Whole stock, const Choice &choice) -> Flag;

auto erase(Whole stock, Whole row) -> Flag;

auto shape(Whole stock, Whole row, Whole shape) -> Flag;

auto shape(Whole stock, Whole row) -> Whole;

auto take(Whole stock, const Vector<Vector<Float>> &lanes) -> STRING::Hot;

void close();

}  // namespace SOUND::INVENTORY

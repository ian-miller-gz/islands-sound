// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "inventory.hpp"

namespace SOUND::INVENTORY {

auto held() -> const Inventory &;

auto counted() -> Whole;

auto stirred(Whole stock) -> Whole;

auto touched() -> const Touched &;

void settled();

auto length(const Stock &stock) -> Whole;

auto carried(const Stock &stock) -> Whole;

auto length(Whole stock, Whole length) -> Flag;

void adopt(const Inventory &pool);

auto refuses(Whole kind) -> STRING::Hot;

auto add(const String &name, Whole kind) -> Whole;

auto name(Whole stock, const String &name) -> Flag;

auto score(Whole stock, const Score &content) -> Flag;

auto content(Whole stock, const Stock &clip) -> Flag;

auto take(Whole stock, const String &path) -> STRING::Hot;

struct Heard {
  Whole channels = 0, rate = 0, frames = 0;
  STRING::Hot refusal = "";
};

auto heard(const String &path) -> Heard;

auto aim(Whole stock, const Address &address) -> Flag;

auto plot(Whole stock, const Point &point) -> Flag;

auto turn(Whole stock, const Turn &move) -> Flag;
auto choose(Whole stock, const Choice &choice) -> Flag;

auto erase(Whole stock, Whole row) -> Flag;

auto shape(Whole stock, Whole row, Whole shape) -> Flag;

auto shape(Whole stock, Whole row) -> Whole;

auto bend(Whole stock, Whole row, Float bend) -> Flag;

auto take(Whole stock, const Vector<Vector<Float>> &lanes) -> STRING::Hot;

void close();

}  // namespace SOUND::INVENTORY

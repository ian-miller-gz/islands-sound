// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

namespace {

SOUND::Inventory pool;

template <typename Row>
void ordered(Vector<Row> &rows, const Row &row) {
  Whole at = rows.size();
  while (at > 0 && row.at < rows[at - 1].at) --at;
  rows.insert(rows.begin() + static_cast<Integer>(at), row);
}

template <typename Row>
auto taken(Vector<Row> &rows, Whole row) -> Flag {
  if (row >= rows.size()) return false;
  rows.erase(rows.begin() + static_cast<Integer>(row));
  return true;
}

auto carrying(Whole index, Whole kind) -> SOUND::Stock * {
  if (index >= ::pool.stocks.size()) return nullptr;
  SOUND::Stock &stock = ::pool.stocks[index];
  return stock.kind == kind ? &stock : nullptr;
}

}  // namespace

auto SOUND::INVENTORY::standing() -> Inventory & { return ::pool; }

auto SOUND::INVENTORY::held() -> const Inventory & { return ::pool; }

void SOUND::INVENTORY::adopt(const Inventory &taken) {
  ::pool = taken;
  stir();
}

auto SOUND::INVENTORY::refuses(Whole kind) -> STRING::Hot {
  return kind == KIND::DATA ? "a data stock's content awaits its first speaker"
                            : "";
}

auto SOUND::INVENTORY::add(const String &name, Whole kind) -> Whole {
  if (name.empty() || refuses(kind)[0] != '\0') return NONE;
  ::pool.stocks.push_back({.kind = kind, .name = name});
  stir(::pool.stocks.size() - 1);
  return ::pool.stocks.size() - 1;
}

auto SOUND::INVENTORY::score(Whole stock, const Score &content) -> Flag {
  Stock *held = ::carrying(stock, KIND::NOTES);
  if (held == nullptr) return false;
  held->score = content;
  stir(stock);
  return true;
}

auto SOUND::INVENTORY::aim(Whole stock, const Address &address) -> Flag {
  Stock *held = ::carrying(stock, KIND::LOGIC);
  if (held == nullptr) return false;
  held->curve.address = address;
  stir(stock);
  return true;
}

auto SOUND::INVENTORY::plot(Whole stock, const Point &point) -> Flag {
  Stock *held = ::carrying(stock, KIND::LOGIC);
  if (held == nullptr) return false;
  ::ordered(held->curve.points, point);
  stir(stock);
  return true;
}

auto SOUND::INVENTORY::turn(Whole stock, const Turn &move) -> Flag {
  Stock *held = ::carrying(stock, KIND::CONTROL);
  if (held == nullptr) return false;
  ::ordered(held->turns, move);
  stir(stock);
  return true;
}

auto SOUND::INVENTORY::choose(Whole stock, const Choice &choice) -> Flag {
  Stock *held = ::carrying(stock, KIND::PROGRAM);
  if (held == nullptr) return false;
  ::ordered(held->choices, choice);
  stir(stock);
  return true;
}

auto SOUND::INVENTORY::erase(Whole stock, Whole row) -> Flag {
  if (stock >= ::pool.stocks.size()) return false;
  Stock &held = ::pool.stocks[stock];
  const Flag gone = held.kind == KIND::CONTROL   ? ::taken(held.turns, row)
                    : held.kind == KIND::PROGRAM ? ::taken(held.choices, row)
                                                 : false;
  if (gone) stir(stock);
  return gone;
}

void SOUND::INVENTORY::close() {
  ::pool = {};
  stir();
}

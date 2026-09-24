// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ERASED = "erase";

struct Node {
  Whole stock = NONE, index = NONE;
};

auto gathered(const Vector<Whole> &rows) -> Vector<::Node> {
  const Vector<VIEWS::SIGNALS::Point> &drawn = VIEWS::SIGNALS::shown();
  Vector<::Node> nodes;
  for (const Whole row : rows) {
    if (row >= drawn.size() || drawn[row].index == NONE) continue;
    if (drawn[row].stock >= INVENTORY::held().stocks.size()) continue;
    nodes.push_back({drawn[row].stock, drawn[row].index});
  }
  std::sort(
    nodes.begin(), nodes.end(), [](const ::Node &one, const ::Node &other) {
      return one.index > other.index;
    });
  return nodes;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::chosen() -> Vector<Whole> {
  const String field = board();
  const Vector<Point> &drawn = shown();
  Vector<Whole> rows;
  for (const STRING::Cold &id :
       GUI::NGA::GET::selections(document(), field.c_str())) {
    const Whole row = GUI::SAC::SEAT::row(String(id), field.c_str());
    if (row >= drawn.size() || drawn[row].index == NONE) continue;
    if (std::find(rows.begin(), rows.end(), row) == rows.end())
      rows.push_back(row);
  }
  std::sort(rows.begin(), rows.end());
  return rows;
}

auto SOUND::VIEWS::SIGNALS::erase(GUI::Handle page, const Vector<Whole> &rows)
  -> Whole {
  if (!GUI::SAC::CARRY::GET::carried(page).empty()) return 0;
  const Vector<::Node> nodes = ::gathered(rows);
  if (nodes.empty()) return 0;
  Whole gone = 0;
  HISTORY::begin();
  for (const ::Node &node : nodes) {
    HISTORY::take(node.stock, INVENTORY::held().stocks[node.stock]);
    if (INVENTORY::erase(node.stock, node.index)) ++gone;
    HISTORY::wrote(::ERASED, node.stock, INVENTORY::held().stocks[node.stock]);
  }
  HISTORY::end();
  return gone;
}

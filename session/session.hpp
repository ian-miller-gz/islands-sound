// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"
#include "../graph.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../transport.hpp"

namespace SOUND {

struct Session {
  String name;
  Arrangement arrangement;
  Graph graph;
  History history;
  Inventory inventory;
  Setting setting;
  Vector<Value> values;
  Vector<String> recents;
};

struct Husk {
  String half;
  Whole at = NONE;
  String line;
};

}  // namespace SOUND

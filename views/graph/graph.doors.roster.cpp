// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUSSED = "bus";
constexpr STRING::Hot TRACKED = "track";

}  // namespace

auto SOUND::VIEWS::WIRED::PALETTE::doors() -> Vector<Door> {
  Vector<String> from;
  for (const String &name : GRAPH::offers()) {
    const String where = GRAPH::from(name);
    if (std::find(from.begin(), from.end(), where) == from.end())
      from.push_back(where);
  }
  std::sort(from.begin(), from.end());
  Vector<Door> rows;
  for (const String &where : from) rows.push_back({where, where});
  rows.push_back({String(::BUSSED), String(VIEWS::WIRED::BUSES)});
  rows.push_back({String(::TRACKED), String(VIEWS::WIRED::TRACKS)});
  for (STRING::Hot device :
       {VIEWS::WIRED::SOURCES, VIEWS::WIRED::SINKS, VIEWS::WIRED::NOTED})
    rows.push_back({String(device), String(device)});
  return rows;
}

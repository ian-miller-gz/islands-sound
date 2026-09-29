// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PLUGGED = "plugins";
constexpr STRING::Hot BUSSED = "bus";
constexpr STRING::Hot TRACKED = "track";

}  // namespace

auto SOUND::VIEWS::WIRED::PALETTE::doors() -> Vector<Door> {
  Vector<Door> rows = {{String(::PLUGGED), String()}};
  rows.push_back({String(::BUSSED), String(VIEWS::WIRED::BUSES)});
  rows.push_back({String(::TRACKED), String(VIEWS::WIRED::TRACKS)});
  for (STRING::Hot device :
       {VIEWS::WIRED::SOURCES, VIEWS::WIRED::SINKS, VIEWS::WIRED::NOTED})
    rows.push_back({String(device), String(device)});
  return rows;
}

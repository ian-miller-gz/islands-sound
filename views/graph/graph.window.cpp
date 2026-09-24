// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>
#include <island/gui/window.hpp>

#include "../../boards.hpp"
#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float UNMEASURED = 0.0f;

}  // namespace

auto SOUND::VIEWS::WIRED::window() -> GUI::Extent {
  const GUI::Handle page = VIEWS::document();
  const String id = WIRED::board();
  return GUI::SAC::WINDOW::seen(
    GUI::GET::measured(page, id.c_str()),
    GUI::NGA::GET::zoom(page, id.c_str()));
}

void SOUND::VIEWS::WIRED::bound() {
  const GUI::Handle page = VIEWS::document();
  const String board = WIRED::board();
  const GUI::Extent seen = GUI::GET::measured(page, board.c_str());
  if (seen.w <= ::UNMEASURED || seen.h <= ::UNMEASURED) return;
  const Vector<Node> &nodes = GRAPH::held().nodes;
  GUI::NGA::Bounds walls;
  Flag homed = false;
  for (Whole index = 0; index < nodes.size(); ++index) {
    const Node &node = nodes[index];
    if (node.across == Float(NONE) || node.down == Float(NONE)) continue;
    const GUI::Extent size = GUI::GET::extent(
      page, GUI::SAC::SEAT::named(board.c_str(), index).c_str());
    walls.west = homed ? std::min(walls.west, node.across) : node.across;
    walls.north = homed ? std::min(walls.north, node.down) : node.down;
    const Float east = node.across + size.w, south = node.down + size.h;
    walls.east = homed ? std::max(walls.east, east) : east;
    walls.south = homed ? std::max(walls.south, south) : south;
    homed = true;
  }
  GUI::NGA::set(
    page, board.c_str(),
    homed ? GUI::SAC::WINDOW::walled(walls, seen, window())
          : GUI::NGA::Bounds{});
}

auto SOUND::VIEWS::WIRED::least() -> GUI::Extent {
  const GUI::Handle page = VIEWS::document();
  const String board = WIRED::board();
  const GUI::NGA::Bounds walls = GUI::NGA::GET::bounds(page, board.c_str());
  if (
    walls.east == GUI::NGA::Bounds::NONE ||
    walls.south == GUI::NGA::Bounds::NONE)
    return {::UNMEASURED, ::UNMEASURED};
  const GUI::Extent seen = GUI::GET::measured(page, board.c_str());
  const Float floor = std::min(
    GUI::SAC::WINDOW::least(seen.w, walls.east - walls.west),
    GUI::SAC::WINDOW::least(seen.h, walls.south - walls.north));
  return {floor, floor};
}

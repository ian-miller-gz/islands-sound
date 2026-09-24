// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto clicked(GUI::Handle page, Whole row, Side side, Whole port) -> Flag {
  const String box = GUI::SAC::SEAT::named(VIEWS::WIRED::board().c_str(), row);
  const String id = VIEWS::WIRED::socket(
    box, side == Side::OUT ? VIEWS::WIRED::OUT : VIEWS::WIRED::IN, port);
  return GUI::GET::clicked(page, id.c_str());
}

auto toggled(const String &node, Side side, Whole port) -> Flag {
  if (GRAPH::joined(node, side, port) > 0)
    return GRAPH::slacken(node, side, port) > 0;
  if (GRAPH::slackened(node, side, port) == 0) return false;
  GRAPH::tighten(node, side, port);
  return true;
}

}  // namespace

auto SOUND::VIEWS::WIRED::pinned() -> Flag {
  const GUI::Handle page = document();
  Flag taken = false;
  const Vector<Node> &nodes = GRAPH::held().nodes;
  for (Whole row = 0; row < nodes.size(); ++row) {
    for (Whole port = 0; port < nodes[row].ins.size(); ++port)
      if (::clicked(page, row, Side::IN, port))
        taken |= ::toggled(nodes[row].name, Side::IN, port);
    for (Whole port = 0; port < nodes[row].outs.size(); ++port)
      if (::clicked(page, row, Side::OUT, port))
        taken |= ::toggled(nodes[row].name, Side::OUT, port);
  }
  return taken;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/gui/seat.hpp>

#include "../../graph.hpp"
#include "graph.keys.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ROOTED = "track command unseats track";
constexpr STRING::Hot TAKING = "recorder follows track";

}  // namespace

auto SOUND::VIEWS::WIRED::chosen(GUI::Handle page) -> Vector<String> {
  const String deck = VIEWS::WIRED::board();
  const Vector<Node> &nodes = GRAPH::held().nodes;
  Vector<String> names;
  for (const STRING::Cold &id : GUI::NGA::GET::selections(page, deck.c_str())) {
    const Whole row = GUI::SAC::SEAT::row(String(id), deck.c_str());
    if (row < nodes.size() && !std::ranges::contains(names, nodes[row].name))
      names.push_back(nodes[row].name);
  }
  return names;
}

void SOUND::VIEWS::WIRED::deleted(GUI::Handle page) {
  const Vector<String> nodes = chosen(page);
  if (nodes.empty()) return;
  GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{""});
  for (const String &node : nodes) {
    if (GRAPH::at(node) == NONE) continue;
    if (GRAPH::rooted(node)) {
      GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{::ROOTED});
      continue;
    }
    if (GRAPH::held().nodes[GRAPH::at(node)].seat == Node::RECORD) {
      GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{::TAKING});
      continue;
    }
    VIEWS::WIRED::unseat(node);
  }
}

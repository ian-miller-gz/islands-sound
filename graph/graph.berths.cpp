// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto own(const Node &node) -> String {
  return node.seat == Node::ROOT ? node.name : node.claim;
}

}  // namespace

auto SOUND::GRAPH::home(const String &node, Float across, Float down) -> Flag {
  return home(node, "", across, down);
}

auto SOUND::GRAPH::home(
  const String &node, const String &page, Float across, Float down) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE) return false;
  if (!page.empty() && !rooted(page)) return false;
  Node &held = standing().nodes[stood];
  if (page.empty() || page == ::own(held)) {
    held.across = across;
    held.down = down;
    return true;
  }
  for (Berth &berth : held.berths)
    if (berth.page == page) {
      berth.across = across;
      berth.down = down;
      return true;
    }
  held.berths.push_back({page, across, down});
  return true;
}

auto SOUND::GRAPH::berth(const String &node, const String &page) -> Berth {
  const Whole stood = at(node);
  if (stood == NONE) return {page};
  const Node &held = standing().nodes[stood];
  if (page.empty() || page == ::own(held))
    return {page, held.across, held.down};
  for (const Berth &berth : held.berths)
    if (berth.page == page) return berth;
  return {page};
}

auto SOUND::GRAPH::unberth(const String &node, const String &page) -> Flag {
  const Whole stood = at(node);
  if (stood == NONE || page.empty()) return false;
  Node &held = standing().nodes[stood];
  if (page == ::own(held)) return false;
  return std::erase_if(held.berths, [&](const Berth &berth) {
           return berth.page == page;
         }) != 0;
}

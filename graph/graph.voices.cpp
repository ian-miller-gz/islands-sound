// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "graph.voices.hpp"
#include "graph.hpp"

auto SOUND::GRAPH::GET::voices(const String &node)
  -> Vector<AUDIO::PLUGIN::Voice> {
  const PLUGIN::Offer *offer = offered(node);
  void *seated = instance(node);
  if (offer == nullptr || offer->surface == nullptr || seated == nullptr)
    return {};
  if (offer->surface->voices == nullptr) return {};
  return offer->surface->voices(seated);
}

auto SOUND::GRAPH::playable(const String &node) -> Flag {
  const Whole seat = at(node);
  if (seat == NONE) return false;
  const String &plugin = held().nodes[seat].plugin;
  return std::ranges::contains(takes(plugin), Whole{KIND::NOTES}) &&
         std::ranges::contains(gives(plugin), Whole{KIND::AUDIO});
}

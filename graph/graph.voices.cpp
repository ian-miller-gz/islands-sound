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

auto SOUND::GRAPH::GET::voices(const Vector<String> &nodes)
  -> Vector<AUDIO::PLUGIN::Voice> {
  Vector<AUDIO::PLUGIN::Voice> merged;
  for (const String &node : nodes)
    for (const AUDIO::PLUGIN::Voice &voice : voices(node))
      if (std::ranges::none_of(merged, [&](const AUDIO::PLUGIN::Voice &one) {
            return one.note == voice.note;
          }))
        merged.push_back(voice);
  return merged;
}

auto SOUND::GRAPH::percussive(const String &node) -> Flag {
  const PLUGIN::Offer *offer = offered(node);
  if (offer == nullptr) return false;
  return String{offer->type} == PLUGIN::PERCUSSION ||
         !GET::voices(node).empty();
}

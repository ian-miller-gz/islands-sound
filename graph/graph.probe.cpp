// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

struct Probe {
  String plugin;
  const PLUGIN::Offer *offer = nullptr;
  void *instance = nullptr;
};

Probe probe;

auto reached(const String &plugin) -> void * {
  const Vector<Node> &nodes = GRAPH::held().nodes;
  for (Whole at = 0; at < nodes.size(); ++at)
    if (nodes[at].plugin == plugin && GRAPH::stands()[at].instance != nullptr)
      return GRAPH::stands()[at].instance;
  if (::probe.plugin == plugin) return ::probe.instance;
  GRAPH::unprobed();
  const PLUGIN::Offer *row = PLUGIN::found(plugin);
  if (row == nullptr) return nullptr;
  void *made = row->surface->create(RATE, CHANNELS);
  if (made == nullptr) return nullptr;
  ::probe = {.plugin = plugin, .offer = row, .instance = made};
  return made;
}

}  // namespace

void SOUND::GRAPH::unprobed() {
  if (::probe.instance != nullptr)
    ::probe.offer->surface->destroy(::probe.instance);
  ::probe = {};
}

auto SOUND::GRAPH::parameters(const String &plugin) -> Whole {
  const PLUGIN::Offer *row = PLUGIN::found(plugin);
  if (row == nullptr || row->surface->parameters == nullptr) return 0;
  void *instance = ::reached(plugin);
  return instance == nullptr ? 0 : row->surface->parameters(instance);
}

auto SOUND::GRAPH::described(
  const String &plugin, Whole parameter, AUDIO::PLUGIN::Control &out) -> Flag {
  if (parameter >= parameters(plugin)) return false;
  const AUDIO::PLUGIN::Plug &surface = *PLUGIN::found(plugin)->surface;
  return surface.control != nullptr &&
         surface.control(::reached(plugin), parameter, out);
}

auto SOUND::GRAPH::named(const String &plugin, Whole parameter) -> String {
  if (parameter >= parameters(plugin)) return {};
  const AUDIO::PLUGIN::Plug &surface = *PLUGIN::found(plugin)->surface;
  return surface.name == nullptr ? String()
                                 : surface.name(::reached(plugin), parameter);
}

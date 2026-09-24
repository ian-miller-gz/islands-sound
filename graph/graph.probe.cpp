// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

struct Probe {
  String plug;
  const PLUG::Offer *offer = nullptr;
  void *instance = nullptr;
};

Probe probe;

auto reached(const String &plug) -> void * {
  const Vector<Node> &nodes = GRAPH::held().nodes;
  for (Whole at = 0; at < nodes.size(); ++at)
    if (nodes[at].plug == plug && GRAPH::stands()[at].instance != nullptr)
      return GRAPH::stands()[at].instance;
  if (::probe.plug == plug) return ::probe.instance;
  GRAPH::unprobed();
  const PLUG::Offer *row = PLUG::found(plug);
  if (row == nullptr) return nullptr;
  void *made = row->surface->create(RATE, CHANNELS);
  if (made == nullptr) return nullptr;
  ::probe = {.plug = plug, .offer = row, .instance = made};
  return made;
}

}  // namespace

void SOUND::GRAPH::unprobed() {
  if (::probe.instance != nullptr)
    ::probe.offer->surface->destroy(::probe.instance);
  ::probe = {};
}

auto SOUND::GRAPH::parameters(const String &plug) -> Whole {
  const PLUG::Offer *row = PLUG::found(plug);
  if (row == nullptr || row->surface->parameters == nullptr) return 0;
  void *instance = ::reached(plug);
  return instance == nullptr ? 0 : row->surface->parameters(instance);
}

auto SOUND::GRAPH::described(
  const String &plug, Whole parameter, AUDIO::PLUGIN::Control &out) -> Flag {
  if (parameter >= parameters(plug)) return false;
  const AUDIO::PLUGIN::Plug &surface = *PLUG::found(plug)->surface;
  return surface.control != nullptr &&
         surface.control(::reached(plug), parameter, out);
}

auto SOUND::GRAPH::named(const String &plug, Whole parameter) -> String {
  if (parameter >= parameters(plug)) return {};
  const AUDIO::PLUGIN::Plug &surface = *PLUG::found(plug)->surface;
  return surface.name == nullptr ? String()
                                 : surface.name(::reached(plug), parameter);
}

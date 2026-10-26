// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

auto SOUND::GRAPH::vetted(const String &node, Whole parameter) -> STRING::Hot {
  const Whole stood = at(node);
  if (stood == NONE) return VETTED::NODE;
  const Stand &stand = GRAPH::stand(stood);
  if (stand.offer == nullptr || stand.instance == nullptr)
    return VETTED::PLUGIN;
  const AUDIO::PLUGIN::Plug &surface = *stand.offer->surface;
  if (
    surface.parameters == nullptr ||
    parameter >= surface.parameters(stand.instance))
    return VETTED::PARAMETER;
  return VETTED::CLEAN;
}

auto SOUND::GRAPH::dial(
  const String &node, Whole at, Whole parameter, Float value) -> STRING::Hot {
  const STRING::Hot why = vetted(node, parameter);
  if (why[0] != '\0') return why;
  Vector<AUDIO::PLUGIN::Event> &run = written(stand(GRAPH::at(node))).dialled;
  ordered(run, {AUDIO::PLUGIN::Event::CONTROLLER, at, parameter, value});
  return "";
}

auto SOUND::GRAPH::PLAN::dialled(Whole node)
  -> const Vector<AUDIO::PLUGIN::Event> & {
  static const Vector<AUDIO::PLUGIN::Event> nothing;
  const Vector<Stand *> &stands = played().stands;
  return node < stands.size() ? walked(*stands[node]).dialled : nothing;
}

auto SOUND::GRAPH::remembered(const String &node, Whole parameter) -> Float {
  const PLUGIN::Offer *offer = offered(node);
  if (offer == nullptr || offer->surface->held == nullptr) return 0.0f;
  return offer->surface->held(instance(node), parameter);
}

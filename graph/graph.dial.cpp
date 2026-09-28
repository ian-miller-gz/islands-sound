// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

auto SOUND::GRAPH::vetted(const String &node, Whole parameter) -> STRING::Hot {
  const Whole stood = at(node);
  if (stood == NONE) return "no such node";
  const Stand &stand = stands()[stood];
  if (stand.offer == nullptr || stand.instance == nullptr)
    return "the node seats no plugin";
  const AUDIO::PLUGIN::Plug &surface = *stand.offer->surface;
  if (
    surface.parameters == nullptr ||
    parameter >= surface.parameters(stand.instance))
    return "no such parameter";
  return "";
}

auto SOUND::GRAPH::dial(
  const String &node, Whole at, Whole parameter, Float value) -> STRING::Hot {
  const STRING::Hot why = vetted(node, parameter);
  if (why[0] != '\0') return why;
  Vector<AUDIO::PLUGIN::Event> &run =
    written(stands()[GRAPH::at(node)]).dialled;
  ordered(run, {AUDIO::PLUGIN::Event::CONTROLLER, at, parameter, value});
  return "";
}

auto SOUND::GRAPH::dialled(const String &node)
  -> const Vector<AUDIO::PLUGIN::Event> & {
  static const Vector<AUDIO::PLUGIN::Event> nothing;
  const Whole stood = at(node);
  return stood != NONE ? walked(stands()[stood]).dialled : nothing;
}

auto SOUND::GRAPH::remembered(const String &node, Whole parameter) -> Float {
  const PLUGIN::Offer *offer = offered(node);
  if (offer == nullptr || offer->surface->held == nullptr) return 0.0f;
  return offer->surface->held(instance(node), parameter);
}

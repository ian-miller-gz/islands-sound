// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.instance.hpp"
#include "graph.hpp"

namespace {
using namespace SOUND;

auto surface(const String &node) -> const AUDIO::PLUGIN::Plug * {
  const PLUGIN::Offer *offer = GRAPH::offered(node);
  return offer == nullptr ? nullptr : offer->surface;
}

}  // namespace

auto SOUND::GRAPH::INSTANCE::parameters(const String &node) -> Whole {
  const AUDIO::PLUGIN::Plug *plug = ::surface(node);
  void *seated = GRAPH::instance(node);
  if (plug == nullptr || plug->parameters == nullptr || seated == nullptr)
    return 0;
  return plug->parameters(seated);
}

auto SOUND::GRAPH::INSTANCE::described(
  const String &node, Whole parameter, AUDIO::PLUGIN::Control &out) -> Flag {
  if (parameter >= parameters(node)) return false;
  const AUDIO::PLUGIN::Plug &plug = *::surface(node);
  return plug.control != nullptr &&
         plug.control(GRAPH::instance(node), parameter, out);
}

auto SOUND::GRAPH::INSTANCE::named(const String &node, Whole parameter)
  -> String {
  if (parameter >= parameters(node)) return {};
  const AUDIO::PLUGIN::Plug &plug = *::surface(node);
  return plug.name == nullptr ? String()
                              : plug.name(GRAPH::instance(node), parameter);
}

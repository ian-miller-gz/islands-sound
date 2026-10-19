// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../graph.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;

auto surface(const String &node) -> const AUDIO::PLUGIN::Plug * {
  const PLUGIN::Offer *offer = GRAPH::offered(node);
  return offer == nullptr ? nullptr : offer->surface;
}

}  // namespace

auto SOUND::VIEWS::RACK::counted(const String &node) -> Whole {
  const AUDIO::PLUGIN::Plug *plug = ::surface(node);
  void *instance = GRAPH::instance(node);
  if (plug == nullptr || plug->parameters == nullptr || instance == nullptr)
    return 0;
  return plug->parameters(instance);
}

auto SOUND::VIEWS::RACK::titled(const Row &row) -> String {
  const AUDIO::PLUGIN::Plug *plug = ::surface(row.node);
  if (plug == nullptr || plug->name == nullptr) return {};
  return plug->name(GRAPH::instance(row.node), row.parameter);
}

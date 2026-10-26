// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <island/audio.hpp>

#include "../graph.hpp"
#include "../timeline.hpp"
#include "views.hpp"

namespace {
using namespace SOUND;

auto entered(const String &root) -> String {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    if (track.root == root) return track.name;
  return {};
}

}  // namespace

auto SOUND::VIEWS::named(const String &node) -> String {
  const Whole stood = GRAPH::at(node);
  if (stood != NONE && GRAPH::held().nodes[stood].seat == Node::RECORD)
    return String(GRAPH::RECORDED);
  if (!GRAPH::rooted(node)) return node;
  const String track = ::entered(node);
  return track.empty() ? node : track;
}

namespace {

auto preferred(const String &plugin) -> String {
  const Vector<Whole> takes = GRAPH::takes(plugin);
  const String name = !takes.empty() && takes[0] == KIND::AUDIO
                        ? AUDIO::OUTPUT::GET::preferred()
                        : String();
  return name.empty() ? String(GRAPH::DEFAULT) : name;
}

}  // namespace

auto SOUND::VIEWS::device(const String &node) -> String {
  const Whole stood = GRAPH::at(node);
  if (stood == NONE) return {};
  const Node &held = GRAPH::held().nodes[stood];
  if (held.seat == Node::ROOT || !GRAPH::bound(held.plugin)) return {};
  const String worn =
    held.device.empty() ? ::preferred(held.plugin) : held.device;
  if (held.lane == 0) return worn;
  return std::format("{} {}-{}", worn, held.lane + 1, held.lane + 2);
}

auto SOUND::VIEWS::parameters(const String &plugin) -> Whole {
  return GRAPH::parameters(plugin);
}

auto SOUND::VIEWS::named(const String &plugin, Whole parameter) -> String {
  return GRAPH::named(plugin, parameter);
}

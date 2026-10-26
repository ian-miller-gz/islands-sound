// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "../kind.hpp"
#include "render.governs.hpp"
#include "render.internal.hpp"

auto SOUND::RENDER::wired(const String &root, Whole out, const String &node)
  -> Flag {
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == root && wire.out == out && wire.to == node) return true;
  return false;
}

auto SOUND::RENDER::governs(Whole track, Whole lane) -> String {
  const String plugin = mapped(track, lane);
  if (plugin.empty()) return {};
  const String &root = TIMELINE::held().tracks[track].root;
  for (const Node &node : GRAPH::held().nodes)
    if (node.plugin == plugin && wired(root, lane, node.name)) return node.name;
  return {};
}

auto SOUND::RENDER::overridden(const TIMELINE::Sent &edge) -> Flag {
  if (edge.kind != KIND::CONTROL) return false;
  return CONTROL::overridden(
    {.node = governs(edge.track, edge.lane), .parameter = edge.number});
}

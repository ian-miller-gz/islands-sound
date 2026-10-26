// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"
#include "render.plan.hpp"
#include "render.voice.hpp"

namespace {
using namespace SOUND;

Vector<RENDER::Wave> waves[GRAPH::PLAN::SIDES];

void reserved(Vector<Vector<AUDIO::PLUGIN::Sample>> &lanes) {
  lanes.resize(CHANNELS);
  for (Vector<AUDIO::PLUGIN::Sample> &lane : lanes) lane.reserve(RENDER::BLOCK);
}

void sized(RENDER::Wave &wave, const Node &node) {
  ::reserved(wave.lanes);
  wave.ins.resize(RENDER::kept(node));
  for (Vector<Vector<AUDIO::PLUGIN::Sample>> &in : wave.ins) ::reserved(in);
  wave.outs.resize(node.outs.size());
}

}  // namespace

auto SOUND::RENDER::carried() -> Vector<Wave> & {
  return ::waves[GRAPH::PLAN::side()];
}

auto SOUND::RENDER::kept(const Node &node) -> Whole {
  if (node.seat != Node::PLUGIN) return node.ins.size();
  for (Whole in = 0; in < node.ins.size(); ++in)
    if (side(node, in)) return node.ins.size();
  return 0;
}

void SOUND::RENDER::plan() {
  const Flag alone = !VOICE::standing();
  if (alone) GRAPH::PLAN::adopt();
  const Whole spare = GRAPH::PLAN::spare();
  if (spare == NONE) return;
  const Vector<Node> &nodes = GRAPH::held().nodes;
  ::waves[spare].resize(nodes.size());
  for (Whole node = 0; node < nodes.size(); ++node)
    ::sized(::waves[spare][node], nodes[node]);
  GRAPH::plan(order(), ordered());
  if (alone) GRAPH::PLAN::adopt();
}

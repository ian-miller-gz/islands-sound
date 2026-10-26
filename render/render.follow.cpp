// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../transport.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole SLACK = 4 * RENDER::BLOCK;

auto opened(const String &root, Whole out, Whole at) -> Vector<Whole> {
  Vector<Whole> pitches;
  for (const AUDIO::PLUGIN::Event &edge : GRAPH::developed(root, out)) {
    if (edge.offset > at) break;
    if (edge.kind == AUDIO::PLUGIN::Event::NOTE_ON)
      pitches.push_back(edge.index);
    else if (edge.kind == AUDIO::PLUGIN::Event::NOTE_OFF)
      std::erase(pitches, edge.index);
  }
  return pitches;
}

auto beginning(const String &root, Whole out, Whole at) -> Vector<Whole> {
  Vector<Whole> pitches;
  for (const AUDIO::PLUGIN::Event &edge : GRAPH::developed(root, out)) {
    if (edge.offset > at + ::SLACK) break;
    if (edge.kind == AUDIO::PLUGIN::Event::NOTE_ON && edge.offset > at)
      pitches.push_back(edge.index);
  }
  return pitches;
}

}  // namespace

void SOUND::RENDER::settle() {
  const Whole at = TRANSPORT::marker().position;
  for (const Node &node : GRAPH::held().nodes) {
    if (node.seat != Node::ROOT) continue;
    for (Whole out = 0; out < node.outs.size(); ++out) {
      Vector<Whole> pitches = ::opened(node.name, out, at);
      for (const Whole pitch : ::beginning(node.name, out, at))
        pitches.push_back(pitch);
      for (const Whole pitch : pitches)
        GRAPH::hand(
          node.name, out, {AUDIO::PLUGIN::Event::NOTE_OFF, 0, pitch, 0});
    }
  }
}

void SOUND::RENDER::follow() {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  if (!marker.playing || moved() == threaded()) return;
  if (!GRAPH::spare()) return;
  threaded() = moved();
  if (threading()) GRAPH::publish();
}

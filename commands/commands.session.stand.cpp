// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../graph.hpp"
#include "../render.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"
#include "commands.internal.hpp"

using namespace SOUND;

namespace {

auto mirrored(const String &name) -> Vector<Whole> {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    if (tracks[track].root == name) return TIMELINE::kinds(track);
  return {};
}

auto stood(const Node &planned, const Graph &plan) -> String {
  if (planned.seat == Node::ROOT)
    return GRAPH::root(::mirrored(planned.name), planned.name);
  if (planned.seat == Node::RECORD)
    return GRAPH::record(planned.claim, planned.name);
  if (planned.clock) return GRAPH::clock(planned.device, planned.name);
  if (!planned.device.empty())
    return GRAPH::surface(planned.plugin, planned.device, planned.name);
  return COMMANDS::held(planned, plan);
}

void laned(const Node &planned) {
  for (const Port &in : planned.ins)
    if (in.lane != NONE) GRAPH::admit(planned.name, in.lane, true);
  for (Whole out = 0; out < planned.outs.size(); ++out)
    if (planned.outs[out].bypassed) GRAPH::bypass(planned.name, out, true);
}

void restood(SHELL::Session &session, const Graph &plan) {
  for (const Node &planned : plan.nodes) {
    const String node = ::stood(planned, plan);
    if (node.empty()) {
      session.print(std::format("seat refused: {}", planned.name));
      continue;
    }
    if (planned.seat == Node::ROOT) ::laned(planned);
    if (planned.across != NONE) GRAPH::home(node, planned.across, planned.down);
  }
  for (const Node &planned : plan.nodes)
    for (const Berth &berth : planned.berths)
      GRAPH::home(planned.name, berth.page, berth.across, berth.down);
}

void refilled() {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    if (GRAPH::rooted(track.root) && GRAPH::recorder(track.root).empty())
      GRAPH::record(track.root);
}

void redrawn(SHELL::Session &session, const Graph &plan) {
  for (const Wire &wire : plan.wires) {
    const STRING::Hot refused =
      GRAPH::refusal(wire.from, wire.out, wire.to, wire.in);
    if (refused[0] != '\0')
      session.print(std::format(
        "wire refused: {} {} {} {} {}", wire.from, wire.out, wire.to, wire.in,
        refused));
    else
      GRAPH::hookup(wire.from, wire.out, wire.to, wire.in);
  }
}

void reclaimed(const Graph &plan) {
  for (const Node &planned : plan.nodes)
    if (!planned.claim.empty()) GRAPH::claim(planned.name, planned.claim);
}

void remarked(const Graph &plan) {
  for (const Node &planned : plan.nodes) {
    if (planned.quiet) GRAPH::quiet(planned.name, true);
    if (planned.lane != 0) GRAPH::lane(planned.name, planned.lane);
  }
  RENDER::VOICE::drop();
  RENDER::VOICE::claim();
}

}  // namespace

void SOUND::COMMANDS::stand(SHELL::Session &session, const Graph &plan) {
  ::restood(session, plan);
  ::refilled();
  ::reclaimed(plan);
  ::redrawn(session, plan);
  ::remarked(plan);
}

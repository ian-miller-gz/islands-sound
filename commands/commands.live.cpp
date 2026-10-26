// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "../graph.hpp"
#include "../kind.hpp"
#include "../render.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto aimed(const String &taking) -> Vector<RENDER::TAP::Aim> {
  Vector<RENDER::TAP::Aim> ins;
  const Whole stood = taking.empty() ? NONE : GRAPH::at(taking);
  if (stood == NONE) return ins;
  const Vector<Port> &ports = GRAPH::held().nodes[stood].ins;
  for (Whole in = 0; in < ports.size(); ++in)
    if (
      ports[in].kind == KIND::AUDIO && GRAPH::joined(taking, Side::IN, in) != 0)
      ins.push_back({taking, in});
  return ins;
}

void tapped() {
  for (const RENDER::TAP::Tapped &block : RENDER::TAP::drained())
    CONTROL::tap(block.at, block.in, block.lanes);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  const Whole track = CONTROL::driven();
  const String taking =
    track < tracks.size() ? GRAPH::recorder(tracks[track].root) : String();
  RENDER::TAP::aim(::aimed(taking));
}

void turned(const CONTROL::MAP::Turn &turn) {
  const Address &address = turn.address;
  if (GRAPH::vetted(address.node, address.parameter)[0] != '\0') return;
  const AUDIO::PLUGIN::Plug &surface = *GRAPH::offered(address.node)->surface;
  AUDIO::PLUGIN::Control control;
  if (
    surface.control == nullptr ||
    !surface.control(GRAPH::instance(address.node), address.parameter, control))
    return;
  const Float value = CONTROL::MAP::scaled(control, turn.fraction);
  GRAPH::hand(address.node, address.parameter, value);
  CONTROL::turn(address, value);
}

void route() {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  const Whole track = CONTROL::driven();
  if (track < tracks.size() && !tracks[track].root.empty())
    for (const AUDIO::PLUGIN::Event &edge : CONTROL::arrived())
      GRAPH::hand(tracks[track].root, edge);
  CONTROL::taken();
  for (const CONTROL::MAP::Turn &turn : CONTROL::MAP::drained()) ::turned(turn);
}

}  // namespace

void SOUND::COMMANDS::tend() {
  GRAPH::listen();
  CONTROL::read();
  CONTROL::hear();
  ::tapped();
  ::route();
  TRANSPORT::tend();
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  if (CONTROL::tend(marker.playing, marker.position, TRANSPORT::paced()))
    land();
  RENDER::follow();
}

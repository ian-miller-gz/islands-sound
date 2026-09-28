// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../kind.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

void silence(Vector<Vector<AUDIO::PLUGIN::Sample>> &lanes, Whole frames) {
  lanes.resize(CHANNELS);
  for (Vector<AUDIO::PLUGIN::Sample> &lane : lanes) lane.assign(frames, 0.0f);
}

void sum(
  const Vector<Vector<AUDIO::PLUGIN::Sample>> &from,
  Vector<Vector<AUDIO::PLUGIN::Sample>> &into, Whole frames) {
  for (Whole channel = 0; channel < into.size() && channel < from.size();
       ++channel)
    for (Whole frame = 0; frame < frames; ++frame)
      into[channel][frame] += from[channel][frame];
}

auto sooner(const AUDIO::PLUGIN::Event &one, const AUDIO::PLUGIN::Event &two)
  -> Flag {
  return one.offset < two.offset;
}

auto onto(RENDER::Wave &wave, Whole seat, Whole in)
  -> Vector<Vector<AUDIO::PLUGIN::Sample>> & {
  if (seat == Node::PLUGIN || in >= wave.ins.size()) return wave.lanes;
  return wave.ins[in];
}

void played(
  Vector<Vector<AUDIO::PLUGIN::Sample>> &into, const Wire &wire,
  const RENDER::Pass &pass) {
  const Vector<Vector<Float>> &laid = GRAPH::laid(wire.from, wire.out);
  RENDER::Leg legs[RENDER::LEGS];
  const Whole count = RENDER::legs(pass.walk, pass.frames, legs);
  for (Whole leg = 0; leg < count; ++leg)
    for (Whole channel = 0; channel < into.size() && channel < laid.size();
         ++channel)
      for (Whole frame = 0; frame < legs[leg].frames &&
                            legs[leg].start + frame < laid[channel].size();
           ++frame)
        into[channel][legs[leg].offset + frame] +=
          laid[channel][legs[leg].start + frame];
}

void handed(
  RENDER::Wave &wave, const GRAPH::Intake &intake, Whole out, Whole kind) {
  for (Whole slot = 0; slot < intake.size; ++slot) {
    const GRAPH::Handed &entry = intake.handed[slot];
    if (entry.out != out) continue;
    if (out != NONE && !KIND::carries(kind, entry.edge.kind)) continue;
    wave.events.push_back(
      {entry.edge.kind, 0, entry.edge.index, entry.edge.value});
  }
}

void own(RENDER::Wave &wave, const String &name, const RENDER::Pass &pass) {
  RENDER::Leg legs[RENDER::LEGS];
  const Whole count = RENDER::legs(pass.walk, pass.frames, legs);
  for (Whole leg = 0; leg < count; ++leg)
    for (const AUDIO::PLUGIN::Event &turn : GRAPH::dialled(name))
      if (
        turn.offset >= legs[leg].start &&
        turn.offset - legs[leg].start < legs[leg].frames)
        wave.events.push_back(
          {turn.kind, turn.offset - legs[leg].start + legs[leg].offset,
           turn.index, turn.value});
  ::handed(wave, GRAPH::taken(name), NONE, KIND::CONTROL);
}

void poured(
  Vector<Vector<AUDIO::PLUGIN::Sample>> &into, const Node &source,
  const RENDER::Wave &given, const Wire &wire, const RENDER::Pass &pass) {
  if (!source.outs[wire.out].bypassed) ::played(into, wire, pass);
  for (Whole in = 0; in < source.ins.size() && in < given.ins.size(); ++in)
    if (source.ins[in].lane == wire.out)
      ::sum(given.ins[in], into, pass.frames);
}

auto heard(RENDER::Wave &wave, const Node &sink, Whole in)
  -> Vector<AUDIO::PLUGIN::Event> * {
  if (sink.seat != Node::ROOT) return &wave.events;
  const Whole lane = in < sink.ins.size() ? sink.ins[in].lane : NONE;
  return lane < wave.outs.size() ? &wave.outs[lane] : nullptr;
}

void brought(
  RENDER::Wave &wave, const Node &sink, const Wire &wire,
  const RENDER::Pass &pass) {
  const Whole from = GRAPH::at(wire.from);
  if (from == NONE) return;
  const Node &source = GRAPH::held().nodes[from];
  const Whole kind = source.outs[wire.out].kind;
  const RENDER::Wave &given = RENDER::carried()[from];
  if (kind == KIND::AUDIO) {
    Vector<Vector<AUDIO::PLUGIN::Sample>> &into =
      ::onto(wave, sink.seat, wire.in);
    if (source.seat == Node::ROOT)
      return ::poured(into, source, given, wire, pass);
    return ::sum(given.lanes, into, pass.frames);
  }
  Vector<AUDIO::PLUGIN::Event> *into = ::heard(wave, sink, wire.in);
  if (into == nullptr) return;
  const Vector<AUDIO::PLUGIN::Event> &edges =
    source.seat == Node::ROOT ? given.outs[wire.out] : given.events;
  for (const AUDIO::PLUGIN::Event &edge : edges)
    if (KIND::carries(kind, edge.kind)) into->push_back(edge);
}

void wired(RENDER::Wave &wave, const Node &sink, const RENDER::Pass &pass) {
  const Vector<Wire> &wires = GRAPH::held().wires;
  const Vector<Flag> &dormant = GRAPH::dormant();
  for (Whole row = 0; row < wires.size() && row < dormant.size(); ++row)
    if (wires[row].to == sink.name && !dormant[row])
      ::brought(wave, sink, wires[row], pass);
}

void apart(
  Vector<Vector<Vector<AUDIO::PLUGIN::Sample>>> &ins, Whole count,
  Whole frames) {
  ins.resize(count);
  for (Vector<Vector<AUDIO::PLUGIN::Sample>> &in : ins) ::silence(in, frames);
}

}  // namespace

void SOUND::RENDER::gather(Whole node, const Pass &pass) {
  const Node &held = GRAPH::held().nodes[node];
  Wave &wave = carried()[node];
  ::silence(wave.lanes, pass.frames);
  ::apart(wave.ins, held.seat == Node::PLUGIN ? 0 : held.ins.size(), pass.frames);
  wave.events.clear();
  if (held.seat == Node::ROOT) rooted(node, pass);
  if (GRAPH::quieted(held.name)) return;
  ::own(wave, held.name, pass);
  ::wired(wave, held, pass);
  std::stable_sort(wave.events.begin(), wave.events.end(), ::sooner);
  if (held.seat != Node::ROOT) return;
  for (Vector<AUDIO::PLUGIN::Event> &out : wave.outs)
    std::stable_sort(out.begin(), out.end(), ::sooner);
  for (const Vector<Vector<AUDIO::PLUGIN::Sample>> &in : wave.ins)
    ::sum(in, wave.lanes, pass.frames);
}

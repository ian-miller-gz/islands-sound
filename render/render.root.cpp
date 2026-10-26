// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../kind.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

auto sooner(const AUDIO::PLUGIN::Event &one, const AUDIO::PLUGIN::Event &two)
  -> Flag {
  return one.offset < two.offset;
}

void broken(
  Vector<AUDIO::PLUGIN::Event> &into, const String &root, Whole out,
  const RENDER::Pass &pass) {
  if (pass.walk.start == NONE || pass.jumped)
    return RENDER::STRUCK::cut(root, out, 0, into);
  if (pass.swapped) RENDER::STRUCK::swapped(root, out, pass.walk.start, into);
}

void handed(
  Vector<AUDIO::PLUGIN::Event> &into, const String &root, Whole out,
  Whole kind) {
  const GRAPH::Intake &intake = GRAPH::taken(root);
  for (Whole slot = 0; slot < intake.size; ++slot) {
    const GRAPH::Handed &entry = intake.handed[slot];
    if (entry.out != out || !KIND::carries(kind, entry.edge.kind)) continue;
    into.push_back({entry.edge.kind, 0, entry.edge.index, entry.edge.value});
    if (entry.edge.kind == AUDIO::PLUGIN::Event::NOTE_OFF)
      RENDER::STRUCK::lifted(root, out, entry.edge.index);
  }
}

void taped(
  Vector<AUDIO::PLUGIN::Event> &into, const String &root, Whole out, Whole kind,
  const RENDER::Leg &leg) {
  for (const AUDIO::PLUGIN::Event &edge : GRAPH::developed(root, out)) {
    if (edge.offset < leg.start || edge.offset - leg.start >= leg.frames)
      continue;
    if (!KIND::carries(kind, edge.kind)) continue;
    into.push_back(
      {edge.kind, edge.offset - leg.start + leg.offset, edge.index,
       edge.value});
    RENDER::STRUCK::struck(root, out, edge);
  }
}

auto last(const RENDER::Pass &pass, const RENDER::Leg &leg) -> Whole {
  return pass.frames == 0 ? 0 : std::min(leg.offset, pass.frames - 1);
}

void passed(
  Vector<AUDIO::PLUGIN::Event> &into, const String &root, Whole out,
  const Port &worn, const RENDER::Pass &pass) {
  RENDER::Leg legs[RENDER::LEGS];
  const Whole count = RENDER::legs(pass.walk, pass.frames, legs);
  ::broken(into, root, out, pass);
  ::handed(into, root, out, worn.kind);
  if (worn.bypassed) RENDER::STRUCK::cut(root, out, 0, into);
  for (Whole leg = 0; leg < count && !worn.bypassed; ++leg) {
    if (leg != 0) RENDER::STRUCK::cut(root, out, ::last(pass, legs[leg]), into);
    ::taped(into, root, out, worn.kind, legs[leg]);
  }
  std::stable_sort(into.begin(), into.end(), ::sooner);
}

}  // namespace

void SOUND::RENDER::rooted(Whole node, const Pass &pass) {
  const Node &held = GRAPH::held().nodes[node];
  Wave &wave = carried()[node];
  wave.outs.resize(held.outs.size());
  for (Vector<AUDIO::PLUGIN::Event> &out : wave.outs) out.clear();
  if (GRAPH::quieted(held.name)) return;
  for (Whole out = 0; out < held.outs.size(); ++out)
    ::passed(wave.outs[out], held.name, out, held.outs[out], pass);
}

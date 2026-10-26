// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "render.internal.hpp"
#include "render.plugged.hpp"

namespace {
using namespace SOUND;

Vector<AUDIO::PLUGIN::Sample *> rows;
Vector<AUDIO::PLUGIN::Event> room;

auto carries(const Vector<Port> &ports, Whole kind) -> Flag {
  for (const Port &port : ports)
    if (port.kind == kind) return true;
  return false;
}

void laid(const Node &held, RENDER::Wave &wave) {
  Whole sides = 0;
  for (Whole in = 0; in < held.ins.size(); ++in)
    sides += RENDER::side(held, in) ? 1 : 0;
  ::rows.resize((1 + sides) * wave.lanes.size());
  Whole row = 0;
  for (Vector<AUDIO::PLUGIN::Sample> &lane : wave.lanes)
    ::rows[row++] = lane.data();
  for (Whole in = 0; in < held.ins.size() && in < wave.ins.size(); ++in)
    if (RENDER::side(held, in))
      for (Vector<AUDIO::PLUGIN::Sample> &lane : wave.ins[in])
        ::rows[row++] = lane.data();
}

}  // namespace

void SOUND::RENDER::plugged(Whole node, Whole frames) {
  const Node &held = GRAPH::played().nodes[node];
  const PLUGIN::Offer *row = GRAPH::PLAN::offered(node);
  void *instance = GRAPH::PLAN::instance(node);
  if (row == nullptr) return passed(node);
  if (instance == nullptr) return;
  Wave &wave = carried()[node];
  delivery() += wave.events.size();
  if (
    row->surface->render != nullptr &&
    (::carries(held.ins, KIND::AUDIO) || ::carries(held.outs, KIND::AUDIO) ||
     ::carries(held.ins, KIND::NOTES))) {
    ::laid(held, wave);
    row->surface->render(
      instance, ::rows.data(), frames, wave.events.data(), wave.events.size());
  }
  if (row->answer == nullptr || !::carries(held.outs, KIND::NOTES))
    return wave.events.clear();
  ::room.resize(PLUGIN::ROOM);
  const Whole said = row->answer(
    instance, wave.events.data(), wave.events.size(), ::room.data(),
    PLUGIN::ROOM);
  wave.events.assign(::room.begin(), ::room.begin() + said);
}

void SOUND::RENDER::passed(Whole node) {
  const Node &held = GRAPH::played().nodes[node];
  Wave &wave = carried()[node];
  if (!::carries(held.ins, KIND::AUDIO) || !::carries(held.outs, KIND::AUDIO))
    for (Vector<AUDIO::PLUGIN::Sample> &lane : wave.lanes)
      std::fill(lane.begin(), lane.end(), 0.0f);
  if (!::carries(held.outs, KIND::NOTES)) wave.events.clear();
}

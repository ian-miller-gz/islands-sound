// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"

namespace {
using namespace SOUND;

Vector<RENDER::Wave> waves;
Whole handed = 0;
Vector<AUDIO::PLUGIN::Sample *> rows;
Vector<AUDIO::PLUGIN::Event> room;

auto carries(const Vector<Port> &ports, Whole kind) -> Flag {
  for (const Port &port : ports)
    if (port.kind == kind) return true;
  return false;
}

void plugged(Whole node, Whole frames) {
  const Node &held = GRAPH::held().nodes[node];
  const PLUGIN::Offer *row = GRAPH::offered(held.name);
  void *instance = GRAPH::instance(held.name);
  if (row == nullptr || instance == nullptr) return;
  RENDER::Wave &wave = RENDER::carried()[node];
  RENDER::delivery() += wave.events.size();
  if (
    row->surface->render != nullptr &&
    (::carries(held.ins, KIND::AUDIO) || ::carries(held.outs, KIND::AUDIO) ||
     ::carries(held.ins, KIND::NOTES))) {
    ::rows.resize(wave.lanes.size());
    for (Whole channel = 0; channel < wave.lanes.size(); ++channel)
      ::rows[channel] = wave.lanes[channel].data();
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

}  // namespace

auto SOUND::RENDER::carried() -> Vector<Wave> & { return ::waves; }
auto SOUND::RENDER::delivery() -> Whole & { return ::handed; }
auto SOUND::RENDER::delivered() -> Whole { return ::handed; }

auto SOUND::RENDER::sounded(const String &node)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> & {
  static const Vector<Vector<AUDIO::PLUGIN::Sample>> nothing;
  const Whole stood = GRAPH::at(node);
  return stood < ::waves.size() ? ::waves[stood].lanes : nothing;
}

auto SOUND::RENDER::landed(const String &node, Whole in)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> & {
  static const Vector<Vector<AUDIO::PLUGIN::Sample>> nothing;
  const Whole stood = GRAPH::at(node);
  if (stood >= ::waves.size() || in >= ::waves[stood].ins.size())
    return nothing;
  return ::waves[stood].ins[in];
}

auto SOUND::RENDER::block(const Walk &walk, Whole frames) -> Flag {
  if (!ordered()) return false;
  GRAPH::take();
  const Pass pass = {
    .walk = walk,
    .frames = frames,
    .jumped = STRUCK::jumped(walk, frames),
    .swapped = GRAPH::adopt()};
  TAP::adopt();
  ::waves.resize(GRAPH::held().nodes.size());
  ::handed = 0;
  for (const Whole node : order()) {
    gather(node, pass);
    const Node &held = GRAPH::held().nodes[node];
    if (held.quiet) continue;
    if (held.seat == Node::PLUGIN) ::plugged(node, frames);
  }
  if (walk.start != NONE) TAP::fed(walk.start, frames);
  return true;
}

auto SOUND::RENDER::block(Whole start, Whole frames) -> Flag {
  return block(Walk{.start = start}, frames);
}

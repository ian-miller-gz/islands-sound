// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"
#include "render.plugged.hpp"

namespace {
using namespace SOUND;

Whole handed = 0;

}  // namespace

auto SOUND::RENDER::delivery() -> Whole & { return ::handed; }
auto SOUND::RENDER::delivered() -> Whole { return ::handed; }

auto SOUND::RENDER::sounded(const String &node)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> & {
  static const Vector<Vector<AUDIO::PLUGIN::Sample>> nothing;
  const Whole stood = GRAPH::PLAN::at(node);
  return stood < carried().size() ? carried()[stood].lanes : nothing;
}

auto SOUND::RENDER::landed(const String &node, Whole in)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> & {
  static const Vector<Vector<AUDIO::PLUGIN::Sample>> nothing;
  const Whole stood = GRAPH::PLAN::at(node);
  if (stood >= carried().size() || in >= carried()[stood].ins.size())
    return nothing;
  return carried()[stood].ins[in];
}

auto SOUND::RENDER::block(const Walk &walk, Whole frames) -> Flag {
  GRAPH::PLAN::adopt();
  const GRAPH::Plan &plan = GRAPH::played();
  if (!plan.whole) return false;
  GRAPH::take();
  const Pass pass = {
    .walk = walk,
    .frames = frames,
    .jumped = STRUCK::jumped(walk, frames),
    .swapped = GRAPH::adopt()};
  TAP::adopt();
  ::handed = 0;
  for (const Whole node : plan.order) {
    gather(node, pass);
    const Node &held = plan.nodes[node];
    if (held.quiet) continue;
    if (held.seat == Node::PLUGIN) plugged(node, frames);
  }
  if (walk.start != NONE) TAP::fed(walk.start, frames);
  return true;
}

auto SOUND::RENDER::block(Whole start, Whole frames) -> Flag {
  return block(Walk{.start = start}, frames);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <metrics.hpp>

#include "../control.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../plugin.hpp"
#include "../transport.hpp"
#include "render.governs.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

Whole stamp = 0;

auto reach() -> Whole {
  Whole end = 0;
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        end = std::max(end, span.at + span.frames);
  return end;
}

void fed(const Vector<TIMELINE::Sounding> &run) {
  for (const TIMELINE::Sounding &note : run)
    GRAPH::develop(
      note.root, note.lane, note.at, note.pitch, note.velocity, note.length);
}

void fed(const Vector<TIMELINE::Laid> &run) {
  for (const TIMELINE::Laid &stretch : run)
    GRAPH::develop(stretch.root, stretch.lane, stretch.at, stretch.lanes);
}

void aimed(const TIMELINE::Sent &edge) {
  const AUDIO::PLUGIN::Event event = RENDER::edged(edge, edge.at);
  const String node =
    edge.kind == KIND::CONTROL ? RENDER::governs(edge.track, edge.lane) : "";
  if (node.empty())
    GRAPH::develop(edge.root, edge.lane, event);
  else
    GRAPH::dial(node, edge.at, edge.number, event.value);
}

void fed(const Vector<TIMELINE::Sent> &run) {
  const Flag marked = !CONTROL::overrides().empty();
  for (const TIMELINE::Sent &edge : run)
    if (!marked || !RENDER::overridden(edge)) ::aimed(edge);
}

void fed(const Vector<TIMELINE::Dialled> &run) {
  for (const TIMELINE::Dialled &turn : run)
    if (!CONTROL::overridden(turn.address))
      GRAPH::dial(
        turn.address.node, turn.at, turn.address.parameter, turn.value);
}

}  // namespace

auto SOUND::RENDER::threaded() -> Whole & { return ::stamp; }

auto SOUND::RENDER::moved() -> Whole {
  return HISTORY::counted() + INVENTORY::counted();
}

auto SOUND::RENDER::threading() -> Flag {
  METRICS::Scope span("sound.thread");
  const Vector<String> owed = claims();
  if (owed.empty()) return false;
  for (const String &node : owed) GRAPH::clear(node);
  const Whole frames = ::reach();
  const Inventory &pool = INVENTORY::held();
  ::fed(TIMELINE::roll(pool, 0, frames, TRANSPORT::held().tempos, owed));
  ::fed(TIMELINE::lay(pool, 0, frames, owed));
  ::fed(sent(pool, 0, frames, owed));
  ::fed(TIMELINE::dial(pool, 0, frames, RENDER::BLOCK, owed));
  rested(owed);
  GRAPH::sort(owed);
  for (const String &node : owed) GRAPH::threaded(node);
  return true;
}

void SOUND::RENDER::spool() {
  threading();
  ::stamp = moved();
}

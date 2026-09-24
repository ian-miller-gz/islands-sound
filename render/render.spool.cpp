// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <metrics.hpp>

#include "../history.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../plug.hpp"
#include "../transport.hpp"
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

void fed(const Vector<TIMELINE::Sent> &run) {
  for (const TIMELINE::Sent &edge : run)
    GRAPH::develop(edge.root, edge.lane, RENDER::edged(edge, edge.at));
}

void fed(const Vector<TIMELINE::Dialled> &run) {
  for (const TIMELINE::Dialled &turn : run)
    GRAPH::dial(turn.address.node, turn.at, turn.address.parameter, turn.value);
}

}  // namespace

auto SOUND::RENDER::edged(const TIMELINE::Sent &edge, Whole at)
  -> AUDIO::PLUGIN::Event {
  if (edge.kind == KIND::CONTROL)
    return {
      AUDIO::PLUGIN::Event::CONTROLLER, at, edge.number,
      worded(edge.track, edge.lane, edge.number, edge.value)};
  return {AUDIO::PLUGIN::Event::PROGRAM, at, edge.number, 0};
}

auto SOUND::RENDER::walk(Whole start, Whole frames) -> Walk {
  const Span cycle =
    TRANSPORT::held().looping ? TRANSPORT::cycled(start) : Span{};
  return walk(start, frames, cycle);
}

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
  GRAPH::sort(owed);
  for (const String &node : owed) GRAPH::threaded(node);
  return true;
}

void SOUND::RENDER::spool() {
  threading();
  ::stamp = moved();
}

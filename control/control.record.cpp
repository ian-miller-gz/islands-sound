// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

namespace {
using namespace SOUND;

void seal(CONTROL::Live &live, Whole pitch) {
  if (!live.holding[pitch]) return;
  live.holding[pitch] = false;
  const Whole from = live.opened[pitch];
  const Whole stands = CONTROL::reached(live);
  live.take.notes.push_back(
    {.pitch = pitch,
     .at = from,
     .length = stands > from ? stands - from : 0,
     .velocity = live.force[pitch]});
}

void begin(CONTROL::Live &live) {
  live.taking = true;
  live.take = {.at = live.position, .tempo = live.tempo};
  for (Whole pitch = 0; pitch < CONTROL::PITCHES; ++pitch)
    live.holding[pitch] = false;
}

auto crossed(const CONTROL::Take &take) -> Whole {
  Whole longest = 0;
  for (const CONTROL::Crossed &held : take.tapped)
    if (!held.lanes.empty() && longest < held.lanes[0].size())
      longest = held.lanes[0].size();
  return longest;
}

auto lanes(const CONTROL::Take &take) -> Vector<Whole> {
  Vector<Whole> crossed;
  for (const CONTROL::Crossed &held : take.tapped) crossed.push_back(held.lane);
  return crossed;
}

void finish(CONTROL::Live &live) {
  for (Whole pitch = 0; pitch < CONTROL::PITCHES; ++pitch) ::seal(live, pitch);
  live.take.frames = CONTROL::reached(live);
  live.taking = false;
  live.ready = true;
}

}  // namespace

auto SOUND::CONTROL::reached(const Live &live) -> Whole {
  return live.position > live.take.at ? live.position - live.take.at : 0;
}

void SOUND::CONTROL::capture(const AUDIO::PLUGIN::Event &edge) {
  Live &live = standing();
  if (!live.taking || edge.index >= PITCHES) return;
  ::seal(live, edge.index);
  if (edge.kind != AUDIO::PLUGIN::Event::NOTE_ON) return;
  live.holding[edge.index] = true;
  live.opened[edge.index] = reached(live);
  live.force[edge.index] = edge.value;
}

void SOUND::CONTROL::arm(Flag on) {
  standing().armed = on;
  listening(on);
}

auto SOUND::CONTROL::reel() -> Reel {
  const Live &live = standing();
  return {
    .armed = live.armed,
    .taking = live.taking,
    .at = live.take.at,
    .frames = live.taking ? reached(live) : live.take.frames,
    .notes = live.take.notes.size(),
    .heard = live.take.heard.empty() ? 0 : live.take.heard[0].size(),
    .tapped = ::crossed(live.take),
    .turned = live.take.turned.size(),
    .crossed = ::lanes(live.take)};
}

auto SOUND::CONTROL::tend(Flag rolling, Whole position, Float tempo) -> Flag {
  Live &live = standing();
  live.position = position;
  live.tempo = tempo;
  const Flag running = live.armed && rolling;
  if (running && !live.taking) ::begin(live);
  if (live.taking && !running) ::finish(live);
  const Flag closed = live.ready;
  live.ready = false;
  return closed;
}

auto SOUND::CONTROL::landed() -> const Take & { return standing().take; }

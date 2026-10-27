// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"
#include "control.map.hpp"

namespace {
using namespace SOUND;

constexpr Whole ROOM = 64;

CONTROL::Live live;

}  // namespace

auto SOUND::CONTROL::standing() -> Live & { return ::live; }

void SOUND::CONTROL::drive(Whole track) { drive(track, NONE); }

void SOUND::CONTROL::drive(Whole track, Whole lane) {
  if (::live.track != track) lift();
  ::live.track = track;
  ::live.lane = lane;
}

auto SOUND::CONTROL::driven() -> Whole { return ::live.track; }

auto SOUND::CONTROL::laned() -> Whole { return ::live.lane; }

void SOUND::CONTROL::aim(const String &node) { ::live.aim = node; }

auto SOUND::CONTROL::aimed() -> String { return ::live.aim; }

void SOUND::CONTROL::settle(const Vector<String> &gone) {
  for (const String &name : gone)
    if (::live.aim == name) ::live.aim.clear();
  MAP::settle(gone);
  lift();
}

auto SOUND::CONTROL::read() -> Whole {
  static Vector<MIDI::Message> wire(ROOM);
  const Whole count = MIDI::read(wire);
  Whole folded = 0;
  for (Whole at = 0; at < count; ++at) {
    const Flag noted = wire[at].kind != MIDI::Message::CONTROL;
    if (noted && ::live.track == NONE) continue;
    if (read(wire[at])) ++folded;
  }
  return folded;
}

auto SOUND::CONTROL::read(const MIDI::Message &message) -> Flag {
  if (message.kind == MIDI::Message::CONTROL)
    return MAP::heard(message) || pedal(message);
  if (
    message.kind != MIDI::Message::NOTE_ON &&
    message.kind != MIDI::Message::NOTE_OFF)
    return false;
  const Flag on = message.kind == MIDI::Message::NOTE_ON && message.second > 0;
  const AUDIO::PLUGIN::Event edge = {
    .kind = on ? AUDIO::PLUGIN::Event::NOTE_ON : AUDIO::PLUGIN::Event::NOTE_OFF,
    .offset = 0,
    .index = message.first,
    .value = on ? Float(message.second) / Float(LOUDEST) : 0.0f};
  if (hold(edge)) return true;
  ::live.arrived.push_back(edge);
  capture(edge);
  return true;
}

auto SOUND::CONTROL::arrived() -> const Vector<AUDIO::PLUGIN::Event> & {
  return ::live.arrived;
}

void SOUND::CONTROL::taken() { ::live.arrived.clear(); }

void SOUND::CONTROL::close() {
  lift();
  listening(false);
  ::live = {};
  values().clear();
  MAP::adopt({});
}

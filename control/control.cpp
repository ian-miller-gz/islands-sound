// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole ROOM = 64;

CONTROL::Live live;

}  // namespace

auto SOUND::CONTROL::standing() -> Live & { return ::live; }

void SOUND::CONTROL::drive(Whole track) { ::live.track = track; }

auto SOUND::CONTROL::driven() -> Whole { return ::live.track; }

void SOUND::CONTROL::aim(const String &node) { ::live.aim = node; }

auto SOUND::CONTROL::aimed() -> String { return ::live.aim; }

void SOUND::CONTROL::settle(const Vector<String> &gone) {
  for (const String &name : gone)
    if (::live.aim == name) ::live.aim.clear();
}

auto SOUND::CONTROL::read() -> Whole {
  if (::live.track == NONE) return 0;
  static Vector<MIDI::Message> wire(ROOM);
  const Whole count = MIDI::read(wire);
  Whole folded = 0;
  for (Whole at = 0; at < count; ++at)
    if (read(wire[at])) ++folded;
  return folded;
}

auto SOUND::CONTROL::read(const MIDI::Message &message) -> Flag {
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
  ::live.arrived.push_back(edge);
  capture(edge);
  return true;
}

auto SOUND::CONTROL::arrived() -> const Vector<AUDIO::PLUGIN::Event> & {
  return ::live.arrived;
}

void SOUND::CONTROL::taken() { ::live.arrived.clear(); }

void SOUND::CONTROL::close() {
  listening(false);
  ::live = {};
  values().clear();
}

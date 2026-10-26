// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "control.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole PEDAL = 64;
constexpr Whole HALF = (CONTROL::LOUDEST + 1) / 2;

void send(CONTROL::Live &live, const AUDIO::PLUGIN::Event &edge) {
  live.arrived.push_back(edge);
  CONTROL::capture(edge);
}

void release(CONTROL::Live &live, Whole pitch) {
  const auto held = std::ranges::find_if(
    live.deferred,
    [&](const AUDIO::PLUGIN::Event &edge) { return edge.index == pitch; });
  if (held == live.deferred.end()) return;
  const AUDIO::PLUGIN::Event edge = *held;
  live.deferred.erase(held);
  ::send(live, edge);
}

}  // namespace

auto SOUND::CONTROL::pedal(const MIDI::Message &message) -> Flag {
  if (message.kind != MIDI::Message::CONTROL || message.first != ::PEDAL)
    return false;
  Live &live = standing();
  live.sustained = message.second >= ::HALF;
  if (!live.sustained) lift();
  return true;
}

auto SOUND::CONTROL::hold(const AUDIO::PLUGIN::Event &edge) -> Flag {
  Live &live = standing();
  if (edge.kind == AUDIO::PLUGIN::Event::NOTE_ON) ::release(live, edge.index);
  if (edge.kind != AUDIO::PLUGIN::Event::NOTE_OFF || !live.sustained)
    return false;
  std::erase_if(live.deferred, [&](const AUDIO::PLUGIN::Event &held) {
    return held.index == edge.index;
  });
  live.deferred.push_back(edge);
  return true;
}

void SOUND::CONTROL::lift() {
  Live &live = standing();
  Vector<AUDIO::PLUGIN::Event> run;
  run.swap(live.deferred);
  for (const AUDIO::PLUGIN::Event &edge : run) ::send(live, edge);
}

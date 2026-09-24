// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ON = "on";
constexpr STRING::Hot OFF = "off";
constexpr Whole ONCE = 1;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto stated() -> String {
  const CONTROL::Reel reel = CONTROL::reel();
  const Whole track = CONTROL::driven();
  const String tapped =
    reel.tapped == 0 ? String() : std::format(" tapped {}", reel.tapped);
  return std::format(
    "control track {} record {} take {} at {} frames {} notes {} heard {} "
    "turned {}{}",
    track == NONE ? String("none") : std::to_string(track),
    reel.armed ? "armed" : "off", reel.taking ? "running" : "idle", reel.at,
    reel.frames, reel.notes, reel.heard, reel.turned, tapped);
}

}  // namespace

void SOUND::COMMANDS::beat(SHELL::Session &session) {
  const Whole times =
    session.arguments.size() > 1 ? ::counted(session.arguments[1]) : ::ONCE;
  for (Whole tended = 0; tended < times; ++tended) tend();
  session.print(std::format("beat {}", times));
}

void SOUND::COMMANDS::drive(SHELL::Session &session) {
  if (session.arguments.size() > 1) {
    const Whole track = ::counted(session.arguments[1]);
    if (track >= TIMELINE::held().tracks.size())
      return session.print("drive refused: no such track");
    CONTROL::drive(track);
  }
  tend();
  session.print(::stated());
}

void SOUND::COMMANDS::strike(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("strike <pitch> <force>");
  tend();
  CONTROL::read(MIDI::Message{
    .kind = MIDI::Message::NOTE_ON,
    .channel = 0,
    .first = std::min(::counted(session.arguments[1]), CONTROL::LOUDEST),
    .second = std::min(::counted(session.arguments[2]), CONTROL::LOUDEST)});
  tend();
  session.print(::stated());
}

void SOUND::COMMANDS::hear(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("hear <frames> [level]");
  tend();
  const Whole frames = ::counted(session.arguments[1]);
  const Float level = session.arguments.size() > 2
                        ? std::strtof(session.arguments[2].c_str(), nullptr)
                        : 1.0f;
  const AUDIO::Sample held =
    static_cast<AUDIO::Sample>(std::clamp(level, -1.0f, 1.0f) * (SCALE - 1.0f));
  const Vector<AUDIO::Sample> block(frames * CHANNELS, held);
  CONTROL::hear(block.data(), frames);
  session.print(::stated());
}

void SOUND::COMMANDS::record(SHELL::Session &session) {
  if (session.arguments.size() > 1) {
    const String &want = session.arguments[1];
    if (want != ON && want != OFF) return session.print("record [on|off]");
    CONTROL::arm(want == ON);
  }
  tend();
  session.print(::stated());
}

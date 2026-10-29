// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.face.hpp"
#include "piano.loudness.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ACT = "loudness";
constexpr STRING::Hot HELP =
  "reads the roll's velocity strip, or shows and hides it: loudness [on|off]";
constexpr STRING::Hot REFUSED = "loudness [on|off]";
constexpr STRING::Hot ON = "on";
constexpr STRING::Hot OFF = "off";

void loudness(SHELL::Session &session) {
  if (session.arguments.size() > 1) {
    const String &want = session.arguments[1];
    if (want != ::ON && want != ::OFF) return session.print(::REFUSED);
    VIEWS::ROLL::loudness(want == ::ON);
  }
  VIEWS::ROLL::loudness(session);
}

}  // namespace

auto SOUND::VIEWS::ROLL::words() -> Vector<SHELL::Command> {
  return {{::ACT, ::HELP, ::loudness}};
}

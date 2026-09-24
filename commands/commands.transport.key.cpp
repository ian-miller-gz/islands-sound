// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../score.hpp"
#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot USAGE =
  "scale <tonic> <name> [at <pulses>] | scale off [at <pulses>]";

auto spelled(const Key &key) -> String {
  Whole degrees = 0;
  for (Whole pitch = 0; pitch < CLASSES; ++pitch)
    if (SCORE::degree(key, pitch) != NONE) ++degrees;
  return degrees == 0 ? std::format("scale {}", ::RESTING)
                      : std::format(
                          "scale {} {} degrees {}", SCORE::classed(key.tonic),
                          key.scale, degrees);
}

void listed(SHELL::Session &session) {
  const Vector<Key> &keys = TRANSPORT::held().keys;
  for (Whole seat = 1; seat < keys.size(); ++seat)
    session.print(
      std::format("{} at {}", ::spelled(keys[seat]), keys[seat].at));
}

auto named(const String &scale) -> Flag {
  for (STRING::Hot row : SCORE::scales())
    if (scale == row) return true;
  return false;
}

auto refused(SHELL::Session &session, const String &word) -> Flag {
  session.print(std::format("scale refused: {}", word));
  return false;
}

auto set(SHELL::Session &session, Whole at) -> Flag {
  const Whole tonic = SCORE::classed(session.arguments[1].c_str());
  if (tonic == NONE) return ::refused(session, session.arguments[1]);
  if (!::named(session.arguments[2]))
    return ::refused(session, session.arguments[2]);
  COMMANDS::keyed(at, tonic, session.arguments[2]);
  return true;
}

}  // namespace

void SOUND::COMMANDS::scale(SHELL::Session &session) {
  const Key none;
  const Asked ask = asked(session);
  if (ask.off && ask.placed) {
    if (!keyed(ask.at)) {
      ::refused(session, std::format("no row at {}", ask.at));
      return;
    }
  } else if (ask.off)
    keyed(0, none.tonic, none.scale);
  else if (ask.words > 2) {
    if (!::set(session, ask.at)) return;
  } else if (ask.words > 1)
    return session.print(::USAGE);
  session.print(::spelled(TRANSPORT::keyed()));
  ::listed(session);
}

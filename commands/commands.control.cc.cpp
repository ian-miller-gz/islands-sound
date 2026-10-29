// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>

#include "../control.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot USAGE = "cc <number> <value>";

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

}  // namespace

void SOUND::COMMANDS::cc(SHELL::Session &session) {
  if (session.arguments.size() < 3) return session.print(::USAGE);
  tend();
  CONTROL::read(MIDI::Message{
    .kind = MIDI::Message::CONTROL,
    .channel = 0,
    .first = std::min(::counted(session.arguments[1]), CONTROL::LOUDEST),
    .second = std::min(::counted(session.arguments[2]), CONTROL::LOUDEST)});
  tend();
  session.print(reeled());
}

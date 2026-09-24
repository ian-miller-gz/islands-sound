// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot FALLBACK = "default";

auto named(const String &device) -> String {
  return device.empty() ? String(::FALLBACK) : device;
}

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

void stated(SHELL::Session &session) {
  for (const AUDIO::INPUT::Device &row : CONTROL::inputs())
    session.print(std::format(
      "input device {} channels {} rate {}", row.name, row.channels, row.rate));
  const CONTROL::Input input = CONTROL::chosen();
  session.print(std::format(
    "input chosen {} channel {}", ::named(input.device), input.channel));
  session.print(std::format(
    "input opening {}",
    ::named(CONTROL::offered(input.device) ? input.device : String())));
}

}  // namespace

void SOUND::COMMANDS::input(SHELL::Session &session) {
  if (session.arguments.size() > 1) CONTROL::choose(session.arguments[1]);
  if (session.arguments.size() > 2)
    CONTROL::choose(::counted(session.arguments[2]));
  ::stated(session);
}

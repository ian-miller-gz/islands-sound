// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <island/audio.hpp>

#include "../graph.hpp"
#include "../render.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto tailed(const SHELL::Session &session, Whole from) -> String {
  String device;
  for (Whole at = from; at < session.arguments.size(); ++at)
    device += (at == from ? "" : " ") + session.arguments[at];
  return device;
}

auto sounds(const String &device) -> Flag {
  if (device.empty()) return true;
  for (const AUDIO::OUTPUT::Device &row : AUDIO::OUTPUT::GET::devices())
    if (row.name == device) return true;
  return false;
}

}  // namespace

void SOUND::COMMANDS::main(SHELL::Session &session) {
  const String clock = GRAPH::clock();
  if (clock.empty()) return session.print("main refused: no clock stands");
  if (session.arguments.size() > 1) {
    const String device =
      session.arguments[1] == GRAPH::DEFAULT ? String() : ::tailed(session, 1);
    if (!::sounds(device))
      return session.print(
        std::format("main refused: no device named {}", device));
    GRAPH::main(device);
    RENDER::VOICE::drop();
    RENDER::VOICE::claim();
  }
  const Node &node = GRAPH::held().nodes[GRAPH::at(clock)];
  session.print(std::format(
    "main {} {} lane {}", clock,
    node.device.empty() ? GRAPH::DEFAULT : node.device, node.lane));
}

void SOUND::COMMANDS::pair(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("pair <node> <first lane>");
  const String &node = session.arguments[1];
  const Whole first = ::counted(session.arguments[2]);
  if (!GRAPH::lane(node, first))
    return session.print(
      std::format("pair refused: {} stands on no device", node));
  if (node == GRAPH::clock()) {
    RENDER::VOICE::drop();
    RENDER::VOICE::claim();
  }
  session.print(std::format("pair {} {}", node, first));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <island/audio.hpp>
#include <island/midi.hpp>

#include "../graph.hpp"
#include "../history.hpp"
#include "../timeline.hpp"
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

auto covered(const String &node) -> Graph {
  return {.nodes = {GRAPH::held().nodes[GRAPH::at(node)]}};
}

auto listed(const String &device) -> Flag {
  for (const AUDIO::INPUT::Device &row : AUDIO::INPUT::GET::devices())
    if (row.name == device) return true;
  for (const AUDIO::OUTPUT::Device &row : AUDIO::OUTPUT::GET::devices())
    if (row.name == device) return true;
  for (const MIDI::Device &row : MIDI::GET::devices())
    if (row.name == device) return true;
  return false;
}

}  // namespace

void SOUND::COMMANDS::io(SHELL::Session &session) {
  for (const AUDIO::INPUT::Device &row : AUDIO::INPUT::GET::devices())
    session.print(std::format(
      "io in {} channels {} rate {}", row.name, row.channels, row.rate));
  for (const AUDIO::OUTPUT::Device &row : AUDIO::OUTPUT::GET::devices())
    session.print(std::format(
      "io out {} channels {} rate {}", row.name, row.channels, row.rate));
  for (const MIDI::Device &row : MIDI::GET::devices())
    session.print(std::format("io midi {}", row.name));
}

void SOUND::COMMANDS::surface(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("surface <track> <input|output|midiout> <device ...>");
  const Whole track = ::counted(session.arguments[1]);
  if (track >= TIMELINE::held().tracks.size())
    return session.print("surface refused: no such track");
  const String root = TIMELINE::held().tracks[track].root;
  if (!GRAPH::rooted(root))
    return session.print("surface refused: that track has no root");
  const String &plugin = session.arguments[2];
  if (!GRAPH::bound(plugin))
    return session.print("surface refused: input, output or midiout");
  const String device = ::tailed(session, 3);
  const String node = GRAPH::surface(plugin, device);
  if (node.empty())
    return session.print(
      ::listed(device)
        ? std::format("surface refused: {} would not open", device)
        : std::format("surface refused: no device named {}", device));
  GRAPH::claim(node, root);
  HISTORY::record({.act = "surface", .graph = ::covered(node)});
  session.print(std::format("surface {} {} on {}", node, plugin, device));
}

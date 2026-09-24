// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../history.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto claim(const String &root) -> Graph {
  Graph facts;
  if (!GRAPH::rooted(root)) return facts;
  facts.nodes.push_back(GRAPH::held().nodes[GRAPH::at(root)]);
  for (const Node &node : GRAPH::held().nodes)
    if (node.claim == root) facts.nodes.push_back(node);
  for (const Wire &wire : GRAPH::held().wires)
    for (const Node &node : facts.nodes)
      if (wire.from == node.name || wire.to == node.name) {
        facts.wires.push_back(wire);
        break;
      }
  return facts;
}

}  // namespace

auto SOUND::COMMANDS::tracked(Whole track) -> String {
  const ARRANGEMENT::Track &held = TIMELINE::held().tracks[track];
  return std::format(
    "track {} {} root {}{}", track, held.name,
    held.root.empty() ? String("none") : held.root, held.bus ? " bus" : "");
}

auto SOUND::COMMANDS::track(const String &name) -> Whole {
  const Whole index = TIMELINE::track(name);
  if (index == NONE) return NONE;
  const String node = root(index, "root");
  TIMELINE::root(index, node);
  GRAPH::record(node);
  const Whole driven = CONTROL::driven();
  if (driven != NONE && driven >= index) CONTROL::drive(driven + 1);
  HISTORY::record(
    {.act = "track",
     .graph = ::claim(node),
     .rows = {{.track = index, .row = TIMELINE::held().tracks[index]}}});
  return index;
}

auto SOUND::COMMANDS::dropped(Whole track) -> Flag {
  if (track >= TIMELINE::held().tracks.size()) return false;
  const String root = TIMELINE::held().tracks[track].root;
  Edit edit = {
    .act = "drop",
    .verb = Edit::REMOVED,
    .graph = ::claim(root),
    .rows = {{.track = track, .row = TIMELINE::held().tracks[track]}}};
  TIMELINE::drop(track);
  const Whole driven = CONTROL::driven();
  if (driven == track)
    CONTROL::drive(NONE);
  else if (driven != NONE && driven > track)
    CONTROL::drive(driven - 1);
  if (GRAPH::rooted(root)) unseated(root, edit.swept);
  HISTORY::record(edit);
  return true;
}

void SOUND::COMMANDS::track(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("track <name>");
  const Whole index = track(session.arguments[1]);
  if (index == NONE) return session.print("track refused: no name");
  session.print(tracked(index));
}

void SOUND::COMMANDS::bus(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("bus <track> [on|off]");
  const Whole track = ::counted(session.arguments[1]);
  if (track >= TIMELINE::held().tracks.size())
    return session.print("bus refused: no such track");
  if (session.arguments.size() > 2) {
    const String &said = session.arguments[2];
    if (said != "on" && said != "off")
      return session.print("bus refused: say on or off");
    bussed(track, said == HISTORY::WORD::ON);
  }
  session.print(tracked(track));
}

void SOUND::COMMANDS::name(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("name <track> [<lane>] <name>");
  const Whole track = ::counted(session.arguments[1]);
  if (track >= TIMELINE::held().tracks.size())
    return session.print("name refused: no such track");
  if (session.arguments.size() > 3) {
    const Whole lane = ::counted(session.arguments[2]);
    if (!named(track, lane, session.arguments[3]))
      return session.print("name refused: no such lane, or no name");
    return session.print(std::format(
      "lane {} {} on track {}", lane,
      TIMELINE::held().tracks[track].lanes[lane].name, track));
  }
  if (!named(track, NONE, session.arguments[2]))
    return session.print("name refused: no name");
  session.print(
    std::format("track {} {}", track, TIMELINE::held().tracks[track].name));
}

void SOUND::COMMANDS::drop(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("drop <track>");
  if (!dropped(::counted(session.arguments[1])))
    return session.print("drop refused: no such track");
  session.print("dropped");
}

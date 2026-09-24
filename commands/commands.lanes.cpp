// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../graph.hpp"
#include "../kind.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto rooted(Whole track) -> String {
  if (track >= TIMELINE::held().tracks.size()) return {};
  const String root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) ? root : String();
}

auto stated(Whole track, Whole lane) -> String {
  const ARRANGEMENT::TRACK::Lane &held =
    TIMELINE::held().tracks[track].lanes[lane];
  const String root = ::rooted(track);
  return std::format(
    "lane {} {} {} on track {} admits {}", lane, held.name,
    KIND::spoken(held.kind), track,
    !root.empty() && GRAPH::admitted(root, lane) != NONE ? "on" : "off");
}

auto switched(const String &said, Flag &on) -> Flag {
  if (said != "on" && said != "off") return false;
  on = said == "on";
  return true;
}

}  // namespace

auto SOUND::COMMANDS::admitted(Whole track, Whole lane, Flag on) -> Flag {
  const String root = ::rooted(track);
  return !root.empty() && GRAPH::admit(root, lane, on);
}

auto SOUND::COMMANDS::recast(Whole track, Whole lane, Whole kind) -> Flag {
  if (!TIMELINE::kind(track, lane, kind)) return false;
  const String root = ::rooted(track);
  if (!root.empty()) GRAPH::recast(root, lane, kind);
  return true;
}

void SOUND::COMMANDS::kind(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("kind <track> <lane> <kind>");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  const Whole kind = KIND::meant(session.arguments[3]);
  if (kind == NONE) return session.print("kind refused: no such kind");
  if (!recast(track, lane, kind))
    return session.print("kind refused: no such lane, or it holds clips");
  session.print(::stated(track, lane));
}

void SOUND::COMMANDS::admit(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("admit <track> <lane> [on|off]");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  if (
    track >= TIMELINE::held().tracks.size() ||
    lane >= TIMELINE::held().tracks[track].lanes.size())
    return session.print("admit refused: no such lane");
  Flag on = false;
  if (session.arguments.size() > 3) {
    if (!::switched(session.arguments[3], on))
      return session.print("admit refused: say on or off");
    if (!admitted(track, lane, on))
      return session.print("admit refused: the track has no root");
  }
  session.print(::stated(track, lane));
}

void SOUND::COMMANDS::bypass(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("bypass <node> <out> [on|off]");
  const String &node = session.arguments[1];
  const Whole out = ::counted(session.arguments[2]);
  if (!GRAPH::rooted(node))
    return session.print("bypass refused: no such root");
  Flag on = false;
  if (session.arguments.size() > 3) {
    if (!::switched(session.arguments[3], on))
      return session.print("bypass refused: say on or off");
    const STRING::Hot refused = GRAPH::bypass(node, out, on);
    if (refused[0] != '\0')
      return session.print(std::format("bypass refused: {}", refused));
  }
  session.print(std::format(
    "bypass {} {} {}", node, out, GRAPH::bypassed(node, out) ? "on" : "off"));
}

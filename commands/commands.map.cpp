// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../history.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto stated(Whole track, Whole lane) -> String {
  const String &map = TIMELINE::held().tracks[track].lanes[lane].map;
  return std::format(
    "map {} {} {}", track, lane, map.empty() ? String("none") : map);
}

}  // namespace

void SOUND::COMMANDS::map(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("map <track> <lane> [plug|none]");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  if (track >= TIMELINE::held().tracks.size())
    return session.print("map refused: no such track");
  if (lane >= TIMELINE::held().tracks[track].lanes.size())
    return session.print("map refused: no such lane");
  if (session.arguments.size() > 3) {
    const String &said = session.arguments[3];
    const String plug = said == "none" ? String() : said;
    const String stood = TIMELINE::held().tracks[track].lanes[lane].map;
    TIMELINE::map(track, lane, plug);
    HISTORY::record(
      {.act = "map",
       .maps = {{.track = track, .lane = lane, .map = plug, .stood = stood}}});
  }
  session.print(::stated(track, lane));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../timeline.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot COUNT = "lane faces {}";
constexpr STRING::Hot LINE =
  "face {} {} pan {} {} zoom {} {} number {} notation {} mapped {}";
constexpr STRING::Hot ABSENT = "none";
constexpr STRING::Hot SEPARATOR = ",";

auto number(Whole value) -> String {
  return value == NONE ? String(::ABSENT) : std::format("{}", value);
}

auto mapped(const Vector<String> &names) -> String {
  String said;
  for (const String &name : names)
    said += (said.empty() ? "" : ::SEPARATOR) + name;
  return said.empty() ? String(::ABSENT) : said;
}

auto stated(const LANE::Face &one) -> String {
  return std::format(
    ::LINE, one.track, one.lane, one.pan.x, one.pan.y, one.zoom.value,
    one.zoom.down, ::number(one.number), one.notation, ::mapped(one.mapped));
}

}  // namespace

auto SOUND::VIEWS::LANE::records() -> std::deque<Face> & {
  static std::deque<Face> held;
  return held;
}

auto SOUND::VIEWS::LANE::faced(const Mark &mark) -> Face & {
  static Face spare;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (mark.track >= tracks.size()) return spare = {};
  const ARRANGEMENT::Track &track = tracks[mark.track];
  if (mark.lane >= track.lanes.size()) return spare = {};
  const String &lane = track.lanes[mark.lane].name;
  for (Face &one : records())
    if (one.track == track.name && one.lane == lane) return one;
  records().push_back({.track = track.name, .lane = lane});
  return records().back();
}

void SOUND::VIEWS::LANE::faces(SHELL::Session &session) {
  if (records().empty()) return;
  session.print(std::format(::COUNT, records().size()));
  for (const Face &one : records()) session.print(::stated(one));
}

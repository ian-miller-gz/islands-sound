// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot ATTENDED = "tracks";
constexpr STRING::Hot LINE = "mark track {} lane {} kind {}";

auto attended() -> Whole {
  const Whole track = VIEWS::cursor(::ATTENDED);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

auto kind(Whole track, Whole lane) -> Whole {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size()) return NONE;
  return tracks[track].lanes[lane].kind;
}

auto first(Whole track, Whole kind) -> LANE::Mark {
  if (track == NONE || kind == NONE) return {};
  const Whole count = TIMELINE::held().tracks[track].lanes.size();
  for (Whole lane = 0; lane < count; ++lane)
    if (::kind(track, lane) == kind) return {track, lane, kind};
  return {};
}

}  // namespace

auto SOUND::VIEWS::LANE::stored() -> Mark & {
  static Mark held;
  return held;
}

void SOUND::VIEWS::LANE::mark(Whole track, Whole lane) {
  const Whole kind = ::kind(track, lane);
  if (kind == NONE) return;
  stored() = {track, lane, kind};
  faced(stored());
}

auto SOUND::VIEWS::LANE::marked() -> Mark {
  const Mark &held = stored();
  const Whole track = ::attended();
  if (track == NONE || held.kind == NONE) return {};
  if (track != held.track) return ::first(track, held.kind);
  return ::kind(held.track, held.lane) == held.kind ? held : Mark{};
}

void SOUND::VIEWS::LANE::marked(SHELL::Session &session) {
  const Mark mark = marked();
  if (mark.lane == NONE) return;
  session.print(
    std::format(::LINE, mark.track, mark.lane, KIND::spoken(mark.kind)));
}

auto SOUND::VIEWS::LANE::edited(Whole kind) -> Mark {
  const Mark mark = marked();
  if (mark.kind == kind) return mark;
  return ::first(::attended(), kind);
}

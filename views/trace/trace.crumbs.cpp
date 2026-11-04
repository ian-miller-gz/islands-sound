// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../arranger/arranger.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.attention.hpp"
#include "trace.face.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;
using ATTENTION::Mark;

auto slot(Whole region) -> ATTENTION::Slot & {
  return ATTENTION::get(ATTENTION::held(region));
}

auto kinded(const ARRANGEMENT::Track &track, Whole lane, Whole kind) -> Flag {
  return lane < track.lanes.size() && track.lanes[lane].kind == kind;
}

auto marked(ATTENTION::Slot &one) -> Mark {
  const Whole track = ATTENTION::attended(one);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || one.mark.kind == NONE) return {};
  const Mark &held = one.mark;
  if (held.track == track && ::kinded(tracks[track], held.lane, held.kind))
    return held;
  for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane)
    if (::kinded(tracks[track], lane, held.kind))
      return {track, lane, held.kind};
  return {};
}

auto named(const ARRANGEMENT::TRACK::Lane &lane) -> String {
  return lane.name.empty() ? String(KIND::spoken(lane.kind)) : lane.name;
}

}  // namespace

auto SOUND::VIEWS::TRACE::crumbs(Whole region) -> Vector<String> {
  ATTENTION::Slot &one = ::slot(region);
  const Whole track = ATTENTION::attended(one);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (ARRANGER::arranged(region) || track >= tracks.size()) return {};
  Vector<String> parts = {tracks[track].name};
  const Mark mark = ::marked(one);
  if (mark.lane != NONE)
    parts.push_back(::named(tracks[track].lanes[mark.lane]));
  const String node = ATTENTION::aimed(one);
  if (!node.empty()) parts.push_back(node);
  return parts;
}

void SOUND::VIEWS::TRACE::unscope(Whole region) {
  ATTENTION::Slot &one = ::slot(region);
  const Mark mark = ::marked(one);
  if (mark.lane == NONE) return;
  LANE::faced(mark).clip = NONE;
  one.clip = NONE;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

auto met(const ARRANGEMENT::TRACK::LANE::Clip &span, Whole opens, Whole closes)
  -> Flag {
  return span.at < closes && opens < span.at + span.frames;
}

}  // namespace

auto SOUND::TIMELINE::bump(
  Whole track, Whole lane, Whole opens, Whole closes,
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &moved,
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &deleted) -> Flag {
  moved.clear();
  deleted.clear();
  Vector<ARRANGEMENT::Track> &tracks = standing().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size())
    return false;
  ARRANGEMENT::TRACK::Lane &held = tracks[track].lanes[lane];
  Vector<ARRANGEMENT::TRACK::LANE::Clip> kept;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : held.clips)
    (::met(span, opens, closes) ? moved : kept).push_back(span);
  held.clips = kept;
  Vector<ARRANGEMENT::TRACK::LANE::Clip> stood;
  for (const ARRANGEMENT::TRACK::LANE::Clip &out : held.outtakes) {
    Flag landed = false;
    for (const ARRANGEMENT::TRACK::LANE::Clip &span : moved)
      if (::met(out, span.at, span.at + span.frames)) landed = true;
    (landed ? deleted : stood).push_back(out);
  }
  held.outtakes = stood;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span : moved)
    held.outtakes.push_back(span);
  return true;
}

auto SOUND::TIMELINE::restore(Whole track, Whole lane, Whole outtake, Whole at)
  -> Whole {
  Vector<ARRANGEMENT::Track> &tracks = standing().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size()) return NONE;
  ARRANGEMENT::TRACK::Lane &held = tracks[track].lanes[lane];
  if (outtake >= held.outtakes.size()) return NONE;
  ARRANGEMENT::TRACK::LANE::Clip span = held.outtakes[outtake];
  span.at = at;
  if (crossed(track, lane, span.at, span.at + span.frames, NONE)) return NONE;
  held.outtakes.erase(held.outtakes.begin() + static_cast<Integer>(outtake));
  held.clips.push_back(span);
  return held.clips.size() - 1;
}

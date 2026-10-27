// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "commands.split.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;

auto met(const Clip &span, Whole opens, Whole closes) -> Flag {
  return span.at < closes && opens < span.at + span.frames;
}

auto aimed(const Placement &one, Whole track, Whole lane) -> Flag {
  return one.track == track && one.lane == lane;
}

auto ignored(
  const Vector<Placement> &ignoring, Whole track, Whole lane, Whole index)
  -> Flag {
  return std::any_of(
    ignoring.begin(), ignoring.end(), [&](const Placement &one) {
      return ::aimed(one, track, lane) && one.placement == index;
    });
}

auto touched(
  const Vector<Placement> &landing, Whole track, Whole lane, const Clip &span)
  -> Flag {
  return std::any_of(landing.begin(), landing.end(), [&](const Placement &one) {
    return ::aimed(one, track, lane) &&
           ::met(span, one.span.at, one.span.at + one.span.frames);
  });
}

auto parted(const Clip &span, Whole kind, Whole opens, Whole closes)
  -> Vector<Clip> {
  if (!::met(span, opens, closes)) return {span};
  Vector<Clip> kept;
  if (span.at < opens)
    kept.push_back(COMMANDS::SPLIT::cut(span, kind, opens)[0]);
  if (closes < span.at + span.frames)
    kept.push_back(COMMANDS::SPLIT::cut(span, kind, closes)[1]);
  return kept;
}

auto outside(
  const Clip &span, Whole kind, const Vector<Placement> &landing, Whole track,
  Whole lane) -> Vector<Clip> {
  Vector<Clip> pieces = {span};
  for (const Placement &one : landing) {
    if (!::aimed(one, track, lane)) continue;
    Vector<Clip> cut;
    for (const Clip &piece : pieces)
      for (const Clip &part : ::parted(
             piece, kind, one.span.at, one.span.at + one.span.frames))
        cut.push_back(part);
    pieces = cut;
  }
  return pieces;
}

void laned(
  COMMANDS::SPLIT::Yield &yield, const Vector<Placement> &landing,
  const Vector<Placement> &ignoring, Whole track, Whole lane) {
  const ARRANGEMENT::TRACK::Lane &held =
    TIMELINE::held().tracks[track].lanes[lane];
  for (Whole index = 0; index < held.clips.size(); ++index) {
    const Clip &span = held.clips[index];
    if (::ignored(ignoring, track, lane, index)) continue;
    if (!::touched(landing, track, lane, span)) continue;
    yield.lifted.push_back({track, lane, index, span});
    for (const Clip &piece : ::outside(span, held.kind, landing, track, lane))
      yield.kept.push_back({track, lane, NONE, piece});
  }
}

}  // namespace

auto SOUND::COMMANDS::SPLIT::yielded(
  const Vector<Placement> &landing, const Vector<Placement> &ignoring)
  -> Yield {
  Yield yield;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane)
      if (std::any_of(
            landing.begin(), landing.end(),
            [&](const Placement &one) { return ::aimed(one, track, lane); }))
        ::laned(yield, landing, ignoring, track, lane);
  return yield;
}

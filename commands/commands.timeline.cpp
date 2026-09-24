// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../history.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../timeline.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto placed(Whole index, const ARRANGEMENT::TRACK::LANE::Clip &span) -> String {
  return std::format(
    "placed {} stock {} at {} from {} for {}", index, span.stock, span.at,
    span.from, span.frames);
}

auto bumped(Whole index, const ARRANGEMENT::TRACK::LANE::Clip &span) -> String {
  return std::format(
    "outtake {} stock {} at {} from {} for {}", index, span.stock, span.at,
    span.from, span.frames);
}

auto standing(Whole track, Whole lane, Whole placement) -> Placement {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size()) return {};
  const Vector<ARRANGEMENT::TRACK::LANE::Clip> &spans =
    tracks[track].lanes[lane].clips;
  if (placement >= spans.size()) return {};
  return {
    .track = track,
    .lane = lane,
    .placement = placement,
    .span = spans[placement]};
}

}  // namespace

void SOUND::COMMANDS::tracks(SHELL::Session &session) {
  const Arrangement &arrangement = TIMELINE::held();
  session.print(std::format("tracks {}", arrangement.tracks.size()));
  for (Whole index = 0; index < arrangement.tracks.size(); ++index) {
    const ARRANGEMENT::Track &track = arrangement.tracks[index];
    session.print(tracked(index));
    for (Whole lane = 0; lane < track.lanes.size(); ++lane) {
      const ARRANGEMENT::TRACK::Lane &held = track.lanes[lane];
      session.print(std::format(
        "lane {} {} {} {} placements", lane, held.name, KIND::spoken(held.kind),
        held.clips.size()));
      for (Whole span = 0; span < held.clips.size(); ++span)
        session.print(::placed(span, held.clips[span]));
      for (Whole out = 0; out < held.outtakes.size(); ++out)
        session.print(::bumped(out, held.outtakes[out]));
    }
  }
}

void SOUND::COMMANDS::lane(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print(
      "lane <track> <audio|notes|control|program|logic|data>");
  const Whole track = ::counted(session.arguments[1]);
  const Whole kind = KIND::meant(session.arguments[2]);
  const Whole lane = laned(track, kind);
  if (lane == NONE) return session.print("lane refused: no such track or kind");
  session.print(
    std::format("lane {} {} on track {}", lane, KIND::spoken(kind), track));
}

void SOUND::COMMANDS::unlane(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("unlane <track> <lane>");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  if (!unlaned(track, lane))
    return session.print("unlane refused: no such track or lane");
  session.print(std::format("unlaned {} on track {}", lane, track));
}

void SOUND::COMMANDS::place(SHELL::Session &session) {
  if (session.arguments.size() < 6)
    return session.print("clip <track> <lane> <stock> <at> <frames> [from]");
  const ARRANGEMENT::TRACK::LANE::Clip span = {
    .stock = ::counted(session.arguments[3]),
    .at = ::counted(session.arguments[4]),
    .from = session.arguments.size() > 6 ? ::counted(session.arguments[6]) : 0,
    .frames = ::counted(session.arguments[5])};
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  const Whole index = TIMELINE::place(INVENTORY::held(), track, lane, span);
  if (index == NONE)
    return session.print("clip refused: no such lane, stock, kind or span");
  HISTORY::record(
    {.act = "clip",
     .placements = {
       {.track = track, .lane = lane, .placement = index, .span = span}}});
  session.print(::placed(index, span));
}

void SOUND::COMMANDS::span(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print(
      "span <track> <lane> <placement> [<at> <frames> [from]]");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  const Whole placement = ::counted(session.arguments[3]);
  const Placement standing = ::standing(track, lane, placement);
  if (standing.placement == NONE)
    return session.print("span refused: no such track, lane or placement");
  if (session.arguments.size() < 6)
    return session.print(::placed(placement, standing.span));
  const ARRANGEMENT::TRACK::LANE::Clip shaped = {
    .stock = standing.span.stock,
    .at = ::counted(session.arguments[4]),
    .from = session.arguments.size() > 6 ? ::counted(session.arguments[6])
                                         : standing.span.from,
    .frames = ::counted(session.arguments[5])};
  if (!TIMELINE::span(track, lane, placement, shaped))
    return session.print("span refused: no span, or it crosses a neighbour");
  HISTORY::record(
    {.act = "span",
     .verb = Edit::CHANGED,
     .placements = {
       {.track = track,
        .lane = lane,
        .placement = placement,
        .span = shaped,
        .was = standing.span}}});
  session.print(::placed(placement, shaped));
}

void SOUND::COMMANDS::restore(SHELL::Session &session) {
  if (session.arguments.size() < 5)
    return session.print("restore <track> <lane> <outtake> <at>");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  const Whole index = TIMELINE::restore(
    track, lane, ::counted(session.arguments[3]),
    ::counted(session.arguments[4]));
  if (index == NONE)
    return session.print("restore refused: no such outtake, or it would cross");
  session.print(
    ::placed(index, TIMELINE::held().tracks[track].lanes[lane].clips[index]));
}

void SOUND::COMMANDS::lift(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("lift <track> <lane> <placement>");
  const Placement covered = ::standing(
    ::counted(session.arguments[1]), ::counted(session.arguments[2]),
    ::counted(session.arguments[3]));
  if (
    covered.track == NONE ||
    !TIMELINE::lift(covered.track, covered.lane, covered.placement))
    return session.print("lift refused: nothing stands there");
  HISTORY::record(
    {.act = "lift", .verb = Edit::REMOVED, .placements = {covered}});
  session.print("lifted");
}

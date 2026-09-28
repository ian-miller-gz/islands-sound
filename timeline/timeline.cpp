// SPDX-License-Identifier: AGPL-3.0-or-later
#include <string>

#include "timeline.internal.hpp"

namespace {

SOUND::Arrangement stage;

auto reached(Whole index) -> SOUND::ARRANGEMENT::Track * {
  return index < ::stage.tracks.size() ? &::stage.tracks[index] : nullptr;
}

auto stamped(const SOUND::ARRANGEMENT::Track &track, Whole kind) -> String {
  const String word = String(SOUND::KIND::spoken(kind));
  for (Whole count = 1;; ++count) {
    const String said = count == 1 ? word : word + std::to_string(count);
    Flag worn = false;
    for (const SOUND::ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      worn = worn || lane.name == said;
    if (!worn) return said;
  }
}

}  // namespace

auto SOUND::TIMELINE::held() -> const Arrangement & { return ::stage; }
auto SOUND::TIMELINE::standing() -> Arrangement & { return ::stage; }

void SOUND::TIMELINE::adopt(const Arrangement &tracks) {
  ::stage = tracks;
  for (ARRANGEMENT::Track &track : ::stage.tracks)
    for (ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      if (lane.name.empty()) lane.name = ::stamped(track, lane.kind);
}

auto SOUND::TIMELINE::track(const String &name) -> Whole {
  if (name.empty()) return NONE;
  Whole index = 0;
  for (Whole at = 0; at < ::stage.tracks.size(); ++at)
    if (!::stage.tracks[at].bus) index = at + 1;
  ::stage.tracks.insert(::stage.tracks.begin() + index, {.name = name});
  for (Whole kind = KIND::AUDIO; kind <= KIND::LOGIC; ++kind) lane(index, kind);
  return index;
}

auto SOUND::TIMELINE::bus(const String &name) -> Whole {
  if (name.empty()) return NONE;
  ::stage.tracks.push_back({.name = name, .bus = true});
  const Whole index = ::stage.tracks.size() - 1;
  lane(index, KIND::AUDIO);
  return index;
}

auto SOUND::TIMELINE::bus(Whole track, Flag on) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr) return false;
  held->bus = on;
  return true;
}

auto SOUND::TIMELINE::kinds(Whole track) -> Vector<Whole> {
  Vector<Whole> carried;
  const ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr) return carried;
  for (const ARRANGEMENT::TRACK::Lane &lane : held->lanes)
    carried.push_back(lane.kind);
  return carried;
}

auto SOUND::TIMELINE::name(Whole track, const String &called) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || called.empty()) return false;
  held->name = called;
  return true;
}

auto SOUND::TIMELINE::name(Whole track, Whole lane, const String &called)
  -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size() || called.empty())
    return false;
  held->lanes[lane].name = called;
  return true;
}

auto SOUND::TIMELINE::kind(Whole track, Whole lane, Whole kind) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size() || kind > KIND::DATA)
    return false;
  ARRANGEMENT::TRACK::Lane &laned = held->lanes[lane];
  if (!laned.clips.empty() || !laned.outtakes.empty()) return false;
  laned.kind = kind;
  laned.map.clear();
  return true;
}

auto SOUND::TIMELINE::drop(Whole track) -> Flag {
  if (::reached(track) == nullptr) return false;
  ::stage.tracks.erase(::stage.tracks.begin() + track);
  return true;
}

auto SOUND::TIMELINE::lane(Whole track, Whole kind) -> Whole {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || kind > KIND::DATA) return NONE;
  held->lanes.push_back({.kind = kind, .name = ::stamped(*held, kind)});
  return held->lanes.size() - 1;
}

auto SOUND::TIMELINE::unlane(Whole track, Whole lane) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return false;
  held->lanes.erase(held->lanes.begin() + lane);
  return true;
}

auto SOUND::TIMELINE::root(Whole track, const String &node) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr) return false;
  held->root = node;
  return true;
}

auto SOUND::TIMELINE::map(Whole track, Whole lane, const String &plugin) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return false;
  held->lanes[lane].map = plugin;
  return true;
}

auto SOUND::TIMELINE::place(
  const Inventory &pool, Whole track, Whole lane,
  const ARRANGEMENT::TRACK::LANE::Clip &span) -> Whole {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return NONE;
  return place(pool, track, lane, held->lanes[lane].clips.size(), span);
}

auto SOUND::TIMELINE::place(
  const Inventory &pool, Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &span) -> Whole {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return NONE;
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &spans = held->lanes[lane].clips;
  if (placement > spans.size()) return NONE;
  if (span.stock >= pool.stocks.size() || span.frames == 0) return NONE;
  if (pool.stocks[span.stock].kind != held->lanes[lane].kind) return NONE;
  spans.insert(spans.begin() + placement, span);
  return placement;
}

auto SOUND::TIMELINE::crossed(
  Whole track, Whole lane, Whole opens, Whole closes, Whole ignoring) -> Flag {
  const ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return false;
  const Vector<ARRANGEMENT::TRACK::LANE::Clip> &spans = held->lanes[lane].clips;
  for (Whole at = 0; at < spans.size(); ++at)
    if (
      at != ignoring && spans[at].at < closes &&
      opens < spans[at].at + spans[at].frames)
      return true;
  return false;
}

auto SOUND::TIMELINE::span(
  Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &shaped) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return false;
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &spans = held->lanes[lane].clips;
  if (placement >= spans.size() || shaped.frames == 0) return false;
  if (crossed(track, lane, shaped.at, shaped.at + shaped.frames, placement))
    return false;
  spans[placement].at = shaped.at;
  spans[placement].from = shaped.from;
  spans[placement].frames = shaped.frames;
  return true;
}

auto SOUND::TIMELINE::lift(Whole track, Whole lane, Whole placement) -> Flag {
  ARRANGEMENT::Track *held = ::reached(track);
  if (held == nullptr || lane >= held->lanes.size()) return false;
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &spans = held->lanes[lane].clips;
  if (placement >= spans.size()) return false;
  spans.erase(spans.begin() + placement);
  return true;
}

void SOUND::TIMELINE::close() { ::stage = {}; }

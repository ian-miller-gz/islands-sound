// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <sstream>

#include "../graph.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

struct Where {
  Whole track = NONE, lane = NONE;
  String node, page;
};

auto rest(std::istringstream &line) -> String {
  String tail;
  std::getline(line >> std::ws, tail);
  return tail;
}

auto said(Flag on) -> String {
  return on ? HISTORY::WORD::ON : HISTORY::WORD::OFF;
}

auto where(const Change &change) -> Where {
  Where found;
  std::istringstream counted(change.at);
  Whole index = 0;
  if (counted >> index) found.track = index;
  if (counted >> index) found.lane = index;
  std::istringstream named(change.at);
  named >> found.node;
  found.page = ::rest(named);
  return found;
}

auto laned(const Where &found) -> const ARRANGEMENT::TRACK::Lane * {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (found.track >= tracks.size()) return nullptr;
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes = tracks[found.track].lanes;
  return found.lane < lanes.size() ? &lanes[found.lane] : nullptr;
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::named(const Change &change) -> String {
  const Where found = ::where(change);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (found.lane != NONE) {
    const ARRANGEMENT::TRACK::Lane *lane = ::laned(found);
    return lane == nullptr ? String() : lane->name;
  }
  return found.track < tracks.size() ? tracks[found.track].name : String();
}

auto SOUND::COMMANDS::CHANGE::named(const Change &change, const String &value)
  -> Flag {
  const Where found = ::where(change);
  return found.lane == NONE ? TIMELINE::name(found.track, value)
                            : TIMELINE::name(found.track, found.lane, value);
}

auto SOUND::COMMANDS::CHANGE::bussed(const Change &change) -> String {
  const Where found = ::where(change);
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  return found.track < tracks.size() ? ::said(tracks[found.track].bus)
                                     : String();
}

auto SOUND::COMMANDS::CHANGE::bussed(const Change &change, const String &value)
  -> Flag {
  return TIMELINE::bus(::where(change).track, value == HISTORY::WORD::ON);
}

auto SOUND::COMMANDS::CHANGE::homed(const Change &change) -> String {
  const Where found = ::where(change);
  const Berth berth = GRAPH::berth(found.node, found.page);
  return berth.across == NONE
           ? String()
           : std::format("{:g} {:g}", berth.across, berth.down);
}

auto SOUND::COMMANDS::CHANGE::homed(const Change &change, const String &value)
  -> Flag {
  const Where found = ::where(change);
  if (value.empty()) return GRAPH::unberth(found.node, found.page);
  std::istringstream spoken(value);
  Float across = 0.0f, down = 0.0f;
  spoken >> across >> down;
  return GRAPH::home(found.node, found.page, across, down);
}

auto SOUND::COMMANDS::CHANGE::quieted(const Change &change) -> String {
  return ::said(GRAPH::quieted(::where(change).node));
}

auto SOUND::COMMANDS::CHANGE::quieted(const Change &change, const String &value)
  -> Flag {
  return GRAPH::quiet(::where(change).node, value == HISTORY::WORD::ON);
}

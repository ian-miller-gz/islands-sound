// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUSSED = "bus";
constexpr STRING::Hot TRACKED = "track";

}  // namespace

auto SOUND::VIEWS::WIRED::PALETTE::doors() -> Vector<Door> {
  Vector<String> from;
  for (const String &name : GRAPH::offers()) {
    const String where = GRAPH::from(name);
    if (std::find(from.begin(), from.end(), where) == from.end())
      from.push_back(where);
  }
  std::sort(from.begin(), from.end());
  Vector<Door> rows;
  for (const String &where : from) rows.push_back({where, where});
  rows.push_back({String(::BUSSED), String(VIEWS::WIRED::BUSES)});
  rows.push_back({String(::TRACKED), String(VIEWS::WIRED::TRACKS)});
  for (STRING::Hot device :
       {VIEWS::WIRED::SOURCES, VIEWS::WIRED::SINKS, VIEWS::WIRED::NOTED})
    rows.push_back({String(device), String(device)});
  return rows;
}

auto SOUND::VIEWS::WIRED::PALETTE::sentence(const Door &door) -> String {
  if (door.word == ::BUSSED)
    return "Bus - the session's buses: the row you take sends this track's "
           "audio into that bus, and the bus stands on this page.";
  if (door.word == ::TRACKED)
    return "Track - the session's other tracks: the row you take sends this "
           "track's audio into that track, and the track stands on this page.";
  if (door.word == VIEWS::WIRED::SOURCES)
    return "In - the soundcard's recording devices: the row you take seats "
           "that device as the track's own input.";
  if (door.word == VIEWS::WIRED::SINKS)
    return "Out - the soundcard's playback devices: the row you take seats "
           "that device as the track's own output.";
  if (door.word == VIEWS::WIRED::NOTED)
    return "MIDI - the soundcard's MIDI devices: the row you take seats that "
           "device as the track's notes out.";
  return std::format(
    "Browse the {} catalog - every plug it offers, with the signature each "
    "would seat with; the row you take is the plug you seat.",
    door.word);
}

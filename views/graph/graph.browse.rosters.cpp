// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <island/audio.hpp>
#include <island/midi.hpp>

#include "../../control.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;
using SOUND::VIEWS::WIRED::NOTED, SOUND::VIEWS::WIRED::SINKS,
  SOUND::VIEWS::WIRED::SOURCES;

void paired(
  Vector<Offer> &rows, STRING::Hot directory, const String &name,
  Whole channels, Whole rate, STRING::Hot way) {
  rows.push_back(
    {.name = name,
     .from = directory,
     .among = directory,
     .reading = std::format("{} {} @ {}", channels, way, rate),
     .device = name});
  for (Whole lane = 2; lane + 1 < std::min(channels, AUDIO::LANES); lane += 2)
    rows.push_back(
      {.name = std::format("{}-{}", lane + 1, lane + 2),
       .from = directory,
       .among = String(directory) + WITHIN + name,
       .reading = std::format("{} {} and {}", way, lane + 1, lane + 2),
       .lane = lane,
       .device = name});
}

}  // namespace

void SOUND::VIEWS::WIRED::BROWSE::recorded(Vector<Offer> &rows) {
  const CONTROL::Input choice = CONTROL::chosen();
  for (const AUDIO::INPUT::Device &device : CONTROL::inputs()) {
    rows.push_back(
      {.name = device.name,
       .from = INPUTS,
       .among = INPUTS,
       .reading = std::format("{} in @ {}", device.channels, device.rate)});
    if (device.name != choice.device) continue;
    for (Whole lane = 0; lane < device.channels; ++lane)
      rows.push_back(
        {.name = std::to_string(lane),
         .from = INPUTS,
         .among = String(INPUTS) + WITHIN + device.name,
         .reading = std::format("lane {} of {}", lane, device.channels),
         .lane = lane});
  }
}

void SOUND::VIEWS::WIRED::BROWSE::surfaced(Vector<Offer> &rows) {
  for (const AUDIO::INPUT::Device &device : AUDIO::INPUT::GET::devices())
    ::paired(rows, SOURCES, device.name, device.channels, device.rate, "in");
  for (const AUDIO::OUTPUT::Device &device : AUDIO::OUTPUT::GET::devices())
    ::paired(rows, SINKS, device.name, device.channels, device.rate, "out");
  for (const MIDI::Device &device : MIDI::GET::devices())
    rows.push_back(
      {.name = device.name,
       .from = NOTED,
       .among = NOTED,
       .reading = "midi out",
       .device = device.name});
}

auto SOUND::VIEWS::WIRED::BROWSE::lacked(Whole track) -> Vector<Offer> {
  Vector<Offer> rows;
  if (track == NONE) return rows;
  const Vector<Whole> carried = TIMELINE::kinds(track);
  const String whose = TIMELINE::held().tracks[track].name;
  for (Whole kind = KIND::AUDIO; kind <= KIND::DATA; ++kind)
    if (!carries(carried, kind))
      rows.push_back(
        {.name = String(KIND::spoken(kind)),
         .from = LANES,
         .among = LANES,
         .reading = std::format("joins {}", whose)});
  return rows;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;
using SOUND::VIEWS::WIRED::BUSES, SOUND::VIEWS::WIRED::TRACKS;

void tracked(Vector<Offer> &rows, const String &steering) {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks) {
    if (track.root == steering || !GRAPH::rooted(track.root)) continue;
    const STRING::Hot directory = track.bus ? BUSES : TRACKS;
    rows.push_back(
      {.name = track.name,
       .from = directory,
       .takes = {KIND::AUDIO},
       .target = track.root,
       .among = directory});
  }
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::before(const Offer &one, const Offer &other)
  -> Flag {
  if (one.from != other.from) return one.from < other.from;
  if (one.among != other.among) return one.among < other.among;
  if (one.shelf != other.shelf) return one.shelf < other.shelf;
  if (one.lane != other.lane) return one.lane < other.lane;
  return one.name < other.name;
}

auto SOUND::VIEWS::WIRED::BROWSE::rows(const String &filing) -> Vector<Offer> {
  const Vector<Offer> plugins = catalogued();
  Vector<Offer> rows;
  ::tracked(rows, VIEWS::WIRED::steering());
  recorded(rows);
  surfaced(rows);
  const Vector<Offer> lanes = lacked(VIEWS::WIRED::steered());
  rows.insert(rows.end(), lanes.begin(), lanes.end());
  if (!filing.empty())
    std::erase_if(rows, [&](const Offer &row) { return row.from != filing; });
  std::sort(rows.begin(), rows.end(), before);
  if (!planted(filing, plugins)) return rows;
  Vector<Offer> level = directories(filing, plugins);
  const Vector<Offer> held = under(filing, plugins);
  level.insert(level.end(), held.begin(), held.end());
  level.insert(level.end(), rows.begin(), rows.end());
  return level;
}

auto SOUND::VIEWS::WIRED::BROWSE::filed(const Offer &offer) -> String {
  return offer.among.empty() ? offer.from + WITHIN + SHELVES[offer.shelf]
                             : offer.among;
}

auto SOUND::VIEWS::WIRED::BROWSE::stated(const Offer &offer) -> String {
  return offer.reading.empty()
           ? std::format(
               "{} {} {}", spelled(offer.takes), INTO, spelled(offer.gives))
           : offer.reading;
}

auto SOUND::VIEWS::WIRED::BROWSE::surfacing(const String &directory) -> String {
  using SOUND::VIEWS::WIRED::NOTED, SOUND::VIEWS::WIRED::SINKS,
    SOUND::VIEWS::WIRED::SOURCES;
  for (const String &name : GRAPH::surfaces()) {
    const Vector<Whole> takes = GRAPH::takes(name), gives = GRAPH::gives(name);
    if (directory == SOURCES && takes.empty() && !gives.empty()) return name;
    if (directory == SINKS && !takes.empty() && takes[0] == KIND::AUDIO)
      return name;
    if (directory == NOTED && !takes.empty() && takes[0] == KIND::NOTES)
      return name;
  }
  return {};
}

auto SOUND::VIEWS::WIRED::BROWSE::devised(const String &directory) -> Flag {
  return directory == SOURCES || directory == SINKS || directory == NOTED;
}

auto SOUND::VIEWS::WIRED::BROWSE::spelled(const Vector<Whole> &kinds)
  -> String {
  String said;
  for (const Whole kind : kinds)
    said += (said.empty() ? "" : " ") + String(KIND::spoken(kind));
  return said.empty() ? String(NOTHING) : said;
}

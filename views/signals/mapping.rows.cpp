// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NONESUCH = "none";
constexpr STRING::Hot FURTHER = "...";
constexpr STRING::Hot NAME = "name";
constexpr STRING::Hot WORDS = "words";
constexpr STRING::Hot PLAIN = "ink";
constexpr STRING::Hot GREY = "muted";
constexpr STRING::Hot BARE = "no params";

auto publishing(const String &name) -> VIEWS::SIGNALS::Offer {
  return {.name = name, .words = VIEWS::parameters(name)};
}

auto landing(const String &root, Whole lane) -> Vector<String> {
  Vector<String> plugins;
  if (root.empty()) return plugins;
  for (const Wire &wire : GRAPH::held().wires) {
    if (wire.from != root || wire.out != lane) continue;
    const Whole stood = GRAPH::at(wire.to);
    if (stood == NONE) continue;
    const String &plugin = GRAPH::held().nodes[stood].plugin;
    if (plugin.empty()) continue;
    if (std::find(plugins.begin(), plugins.end(), plugin) == plugins.end())
      plugins.push_back(plugin);
  }
  std::sort(plugins.begin(), plugins.end());
  return plugins;
}

auto spoken(const VIEWS::SIGNALS::Offer &offer) -> String {
  if (offer.words == NONE) return {};
  if (offer.words == 0) return String(::BARE);
  return std::format("{} params", offer.words);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::reached() -> Vector<Offer> {
  Vector<Offer> rows = {{.name = String(::NONESUCH)}};
  const Whole track = attended();
  const Whole lane = laned();
  if (track != NONE && lane != NONE)
    for (const String &plugin :
         ::landing(TIMELINE::held().tracks[track].root, lane))
      rows.push_back(::publishing(plugin));
  rows.push_back({.name = String(::FURTHER)});
  return rows;
}

auto SOUND::VIEWS::SIGNALS::offered() -> Vector<Offer> {
  Vector<Offer> rows;
  for (const String &plugin : GRAPH::offers()) rows.push_back(::publishing(plugin));
  std::sort(rows.begin(), rows.end(), [](const Offer &one, const Offer &two) {
    return one.name < two.name;
  });
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    if (GRAPH::rooted(track.root)) rows.push_back(::publishing(track.name));
  return rows;
}

auto SOUND::VIEWS::SIGNALS::seats(STRING::Hot id, Whole rows) -> Vector<Whole> {
  const GUI::Handle page = document();
  GUI::set(page, id, GUI::Rows{rows});
  const Whole first = GUI::GET::first(page, id);
  Vector<Whole> shown;
  for (Whole seat = 0; first + seat < rows; ++seat) {
    const String cell = std::format("{}.{}", id, seat);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    shown.push_back(first + seat);
  }
  return shown;
}

void SOUND::VIEWS::SIGNALS::listing(STRING::Hot id, const Vector<Offer> &rows) {
  const GUI::Handle page = document();
  const Vector<Whole> shown = seats(id, rows.size());
  for (Whole seat = 0; seat < shown.size(); ++seat) {
    const String cell = std::format("{}.{}", id, seat);
    const Offer &offer = rows[shown[seat]];
    const String name = cell + "." + ::NAME;
    GUI::set(page, name.c_str(), GUI::Text{offer.name});
    GUI::set(
      page, name.c_str(), GUI::Style{offer.words == 0 ? ::GREY : ::PLAIN});
    GUI::set(page, (cell + "." + ::WORDS).c_str(), GUI::Text{::spoken(offer)});
  }
}

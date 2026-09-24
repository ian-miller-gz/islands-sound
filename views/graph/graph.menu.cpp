// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot UNSEATS = "Delete";
constexpr STRING::Hot TRACKED = "Track";
constexpr STRING::Hot BUSSED = "Bus";
constexpr Whole BUS = 1;
constexpr Whole DISMISS = 2;

struct Subject {
  String node;
  Whole track = NONE;
};
Subject about;

auto subjected(const String &target) -> Whole {
  const String bed = VIEWS::WIRED::board();
  for (Whole node = 0; node < GRAPH::held().nodes.size(); ++node)
    if (GUI::SAC::SEAT::named(bed.c_str(), node) == target) return node;
  return NONE;
}

auto entered(const String &root) -> Whole {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    if (tracks[track].root == root) return track;
  return NONE;
}

void took(Whole row) {
  const ::Subject held = ::about;
  ::about = {};
  if (held.track == NONE) return void(VIEWS::WIRED::unseat(held.node));
  if (row == ::DISMISS) return void(VIEWS::WIRED::dismiss(held.node));
  TIMELINE::bus(held.track, row == ::BUS);
}

void rooted(const Node &held, const GUI::NGA::Ask &ask) {
  const Whole track = ::entered(held.name);
  if (track == NONE) return;
  ::about = {.node = held.name, .track = track};
  Vector<String> rows{String(::TRACKED), String(::BUSSED)};
  if (held.name != VIEWS::WIRED::steering()) rows.push_back(String(::UNSEATS));
  VIEWS::MENU::raise(rows, ask.x, ask.y, "graph");
}

}  // namespace

void SOUND::VIEWS::WIRED::menu() {
  const GUI::Handle page = VIEWS::document();
  if (!::about.node.empty()) {
    const Whole row = VIEWS::MENU::taken("graph");
    if (row != NONE) return ::took(row);
    if (!VIEWS::MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  if (ask.target.empty()) return;
  const Whole node = ::subjected(ask.target);
  if (node == NONE) return;
  const Node &held = GRAPH::held().nodes[node];
  if (held.seat == Node::RECORD) return;
  if (GRAPH::rooted(held.name)) return ::rooted(held, ask);
  ::about = {.node = held.name};
  VIEWS::MENU::raise({String(::UNSEATS)}, ask.x, ask.y, "graph");
}

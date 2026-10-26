// SPDX-License-Identifier: AGPL-3.0-or-later
#include <span>
#include <utility>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot UNSEATS = "Delete";
constexpr STRING::Hot TRACKED = "Track";
constexpr STRING::Hot BUSSED = "Bus";

struct Act {
  STRING::Hot label = "";
  void (*act)(const String &node) = nullptr;
  auto (*shown)(const String &node) -> Flag = nullptr;
};

auto entered(const String &root) -> Whole {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    if (tracks[track].root == root) return track;
  return NONE;
}

void replace(const String &node) { VIEWS::WIRED::replacing(node); }
void unseat(const String &node) { VIEWS::WIRED::unseat(node); }
void track(const String &node) { TIMELINE::bus(::entered(node), false); }
void bus(const String &node) { TIMELINE::bus(::entered(node), true); }
void dismiss(const String &node) { VIEWS::WIRED::dismiss(node); }

auto foreign(const String &node) -> Flag {
  return node != VIEWS::WIRED::steering();
}

constexpr Act PLUGS[] = {
  {.label = VIEWS::WIRED::REPLACES,
   .act = ::replace,
   .shown = VIEWS::WIRED::replaceable},
  {.label = ::UNSEATS, .act = ::unseat}};
constexpr Act ROOTS[] = {
  {.label = ::TRACKED, .act = ::track},
  {.label = ::BUSSED, .act = ::bus},
  {.label = ::UNSEATS, .act = ::dismiss, .shown = ::foreign}};

String about;
Vector<const Act *> run;

auto subjected(const String &target) -> Whole {
  const String bed = VIEWS::WIRED::board();
  for (Whole node = 0; node < GRAPH::held().nodes.size(); ++node)
    if (GUI::SAC::SEAT::named(bed.c_str(), node) == target) return node;
  return NONE;
}

void hang(
  const String &subject, std::span<const Act> acts, const GUI::NGA::Ask &ask) {
  ::about = subject;
  ::run.clear();
  Vector<String> rows;
  for (const Act &act : acts)
    if (act.shown == nullptr || act.shown(subject)) {
      ::run.push_back(&act);
      rows.push_back(String(act.label));
    }
  VIEWS::MENU::raise(rows, ask.x, ask.y, VIEWS::WIRED::WHOSE);
}

void took(Whole row) {
  const Vector<const Act *> held = std::exchange(::run, {});
  if (row < held.size()) held[row]->act(::about);
}

}  // namespace

void SOUND::VIEWS::WIRED::menu() {
  asking();
  if (!::run.empty()) {
    const Whole row = VIEWS::MENU::taken(WHOSE);
    if (row != NONE) return ::took(row);
    if (!VIEWS::MENU::standing()) ::run.clear();
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(document());
  if (ask.board.empty() || ask.board != board() || ask.target.empty()) return;
  const Whole node = ::subjected(ask.target);
  if (node == NONE) return;
  const Node &held = GRAPH::held().nodes[node];
  if (held.seat == Node::RECORD) return;
  if (!GRAPH::rooted(held.name)) return ::hang(held.name, ::PLUGS, ask);
  if (::entered(held.name) != NONE) ::hang(held.name, ::ROOTS, ask);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../control.hpp"
#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ROOTED = "a track leaves by its own command";
constexpr STRING::Hot TAKING = "a track's recorder leaves with its track";

auto dot(Whole node) -> String {
  return GUI::SAC::SEAT::named(VIEWS::WIRED::board().c_str(), node) + "." +
         VIEWS::WIRED::QUIET;
}

auto face(Whole node) -> String {
  return GUI::SAC::SEAT::named(VIEWS::WIRED::board().c_str(), node) + "." +
         VIEWS::WIRED::FACE;
}

void open(const String &node) {
  if (node.empty() || GRAPH::offered(node) == nullptr) return;
  CONTROL::aim(node);
  VIEWS::choose("params");
}

auto standing(Whole row) -> String {
  return row < GRAPH::held().nodes.size() ? GRAPH::held().nodes[row].name
                                          : String();
}

void knobbed(GUI::Handle page, Whole row, const Node &node) {
  if (node.seat != Node::ROOT) return;
  const String box = GUI::SAC::SEAT::named(VIEWS::WIRED::board().c_str(), row);
  for (Whole out = 0; out < node.outs.size(); ++out) {
    if (!GUI::GET::clicked(page, VIEWS::WIRED::knob(box, out).c_str()))
      continue;
    const STRING::Hot refused =
      GRAPH::bypass(node.name, out, !node.outs[out].bypassed);
    GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{refused});
  }
}

void opened(GUI::Handle page) {
  const String deck = VIEWS::WIRED::board();
  if (!GUI::GET::activated(page, deck.c_str())) return;
  ::open(::standing(GUI::SAC::SEAT::selected(page, deck.c_str())));
}

auto chosen(GUI::Handle page) -> Vector<String> {
  const String deck = VIEWS::WIRED::board();
  Vector<String> nodes;
  for (const STRING::Cold &id : GUI::NGA::GET::selections(page, deck.c_str())) {
    const String node =
      ::standing(GUI::SAC::SEAT::row(String(id), deck.c_str()));
    if (!node.empty()) nodes.push_back(node);
  }
  return nodes;
}

void deleted(GUI::Handle page) {
  const INPUT::KEYS::Event key = GUI::GET::keyed(page);
  if (VIEWS::journaled(key)) return;
  if (key.action != INPUT::KEYS::DELETE) return;
  const Vector<String> nodes = ::chosen(page);
  if (nodes.empty()) return;
  GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{""});
  for (const String &node : nodes) {
    if (GRAPH::at(node) == NONE) continue;
    if (GRAPH::rooted(node)) {
      GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{::ROOTED});
      continue;
    }
    if (GRAPH::held().nodes[GRAPH::at(node)].seat == Node::RECORD) {
      GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{::TAKING});
      continue;
    }
    VIEWS::WIRED::unseat(node);
  }
}

}  // namespace

void SOUND::VIEWS::WIRED::acts() {
  if (browsing()) return;
  const GUI::Handle page = document();
  for (Whole row = 0; row < GRAPH::held().nodes.size(); ++row) {
    const String node = GRAPH::held().nodes[row].name;
    if (GUI::GET::clicked(page, ::dot(row).c_str()))
      GRAPH::quiet(node, !GRAPH::quieted(node));
    if (GUI::GET::clicked(page, ::face(row).c_str())) ::open(node);
    ::knobbed(page, row, GRAPH::held().nodes[row]);
  }
  ::opened(page);
  ::deleted(page);
}

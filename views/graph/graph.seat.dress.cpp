// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.browse.internal.hpp"
#include "graph.internal.hpp"
#include "graph.seat.dress.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot TYPED = "node";
constexpr STRING::Hot WORDS[] = {
  "instrument", "effect", "shaper", "controller", "plugin"};
static_assert(std::size(WORDS) == VIEWS::WIRED::BROWSE::PLUGINS + 1);
constexpr STRING::Hot LOST = "missing";

struct Dress {
  auto (*worn)(const Node &node) -> Flag;
  STRING::Hot word;
};

auto rooted(const Node &node) -> Flag { return node.seat == Node::ROOT; }
auto taking(const Node &node) -> Flag { return node.seat == Node::RECORD; }
auto ticking(const Node &node) -> Flag { return node.clock; }
auto lost(const Node &node) -> Flag { return GRAPH::missing(node.name); }

constexpr Dress DRESSES[] = {
  {::rooted, "root"},
  {::taking, "record"},
  {::ticking, "clock"},
  {::lost, ::LOST}};

}  // namespace

auto SOUND::VIEWS::WIRED::dressed(const Node &node) -> String {
  for (const Dress &dress : ::DRESSES)
    if (dress.worn(node)) return String(::TYPED) + dress.word;
  return String(::TYPED) +
         ::WORDS[BROWSE::shelved(
           GRAPH::takes(node.plugin), GRAPH::gives(node.plugin))];
}

auto SOUND::VIEWS::WIRED::surfaced(const Node &node) -> Flag {
  return node.seat == Node::PLUGIN &&
         (GRAPH::bound(node.plugin) || GRAPH::missing(node.name));
}

auto SOUND::VIEWS::WIRED::dotted(const Node &node) -> Flag {
  return node.seat != Node::ROOT && node.seat != Node::RECORD;
}

auto SOUND::VIEWS::WIRED::subtitled(const Node &node) -> String {
  return GRAPH::missing(node.name) ? String(::LOST) : VIEWS::device(node.name);
}

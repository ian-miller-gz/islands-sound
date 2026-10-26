// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../history.hpp"
#include "../../session.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot REPLACED = "replace";
constexpr STRING::Hot WIRED = "wire";
constexpr STRING::Hot REFUSED = "plugin not made";
constexpr STRING::Hot DROPPED = "{} wires dropped";

auto held(const String &node) -> const Node & {
  return GRAPH::held().nodes[GRAPH::at(node)];
}

auto matched(const Vector<Port> &was, Whole port, const Vector<Port> &now)
  -> Whole {
  if (port >= was.size()) return NONE;
  const Whole kind = was[port].kind;
  Whole ordinal = 0;
  for (Whole one = 0; one < port; ++one) ordinal += was[one].kind == kind;
  for (Whole one = 0; one < now.size(); ++one)
    if (now[one].kind == kind && ordinal-- == 0) return one;
  return NONE;
}

void moved(const Node &old, const String &fresh) {
  if (!old.claim.empty()) GRAPH::claim(fresh, old.claim);
  if (old.across != NONE) GRAPH::home(fresh, old.across, old.down);
  for (const Berth &berth : old.berths)
    if (berth.across != NONE)
      GRAPH::home(fresh, berth.page, berth.across, berth.down);
  if (old.quiet) GRAPH::quiet(fresh, true);
}

auto redrawn(const Wire &wire, const Node &old, const String &fresh) -> Flag {
  const Node &now = ::held(fresh);
  Wire drawn = wire;
  if (wire.from == old.name)
    drawn = {
      fresh, ::matched(old.outs, wire.out, now.outs), drawn.to, drawn.in};
  if (wire.to == old.name)
    drawn = {
      drawn.from, drawn.out, fresh, ::matched(old.ins, wire.in, now.ins)};
  if (drawn.out == NONE || drawn.in == NONE) return false;
  if (!GRAPH::hookup(drawn.from, drawn.out, drawn.to, drawn.in)) return false;
  HISTORY::record({.act = ::WIRED, .graph = {.wires = {drawn}}});
  return true;
}

auto redrawn(const Node &old, const String &fresh) -> Whole {
  Vector<Wire> wires;
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == old.name || wire.to == old.name) wires.push_back(wire);
  Whole dropped = 0;
  for (const Wire &wire : wires) dropped += !::redrawn(wire, old, fresh);
  return dropped;
}

auto said(const String &fresh, Whole dropped) -> String {
  if (fresh.empty()) return ::REFUSED;
  return dropped == 0 ? String() : std::format(::DROPPED, dropped);
}

}  // namespace

auto SOUND::VIEWS::WIRED::replace(const String &node, const String &plugin)
  -> Whole {
  if (!replaceable(node)) return 0;
  const Node old = ::held(node);
  HISTORY::begin();
  const String fresh = GRAPH::seat(plugin);
  Whole dropped = 0;
  if (!fresh.empty()) {
    SESSION::seated(plugin);
    ::moved(old, fresh);
    HISTORY::record({.act = ::REPLACED, .graph = {.nodes = {::held(fresh)}}});
    dropped = ::redrawn(old, fresh);
    unseat(node);
  }
  HISTORY::end();
  GUI::set(document(), NOTICE, GUI::Text{::said(fresh, dropped)});
  return dropped;
}

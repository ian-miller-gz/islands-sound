// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto on(const Wire &wire, const String &node, Side side, Whole port) -> Flag {
  return side == Side::OUT ? wire.from == node && wire.out == port
                           : wire.to == node && wire.in == port;
}

auto counted(
  const Vector<Wire> &wires, const String &node, Side side,
  Whole port) -> Whole {
  Whole hanging = 0;
  for (const Wire &wire : wires)
    if (::on(wire, node, side, port)) hanging += 1;
  return hanging;
}

auto taken(Vector<Wire> &wires, const String &node, Side side, Whole port)
  -> Vector<Wire> {
  Vector<Wire> hanging, kept;
  for (const Wire &wire : wires)
    (::on(wire, node, side, port) ? hanging : kept).push_back(wire);
  wires = kept;
  return hanging;
}

}  // namespace

auto SOUND::GRAPH::joined(const String &node, Side side, Whole port) -> Whole {
  return ::counted(held().wires, node, side, port);
}

auto SOUND::GRAPH::slackened(const String &node, Side side, Whole port)
  -> Whole {
  return ::counted(held().slack, node, side, port);
}

auto SOUND::GRAPH::slacken(const String &node, Side side, Whole port) -> Whole {
  Vector<Wire> hanging;
  for (const Wire &wire : held().wires)
    if (::on(wire, node, side, port)) hanging.push_back(wire);
  for (const Wire &wire : hanging) {
    unwire(wire.from, wire.out, wire.to, wire.in);
    standing().slack.push_back(wire);
  }
  return hanging.size();
}

auto SOUND::GRAPH::tighten(const String &node, Side side, Whole port) -> Whole {
  const Vector<Wire> hanging = ::taken(standing().slack, node, side, port);
  Whole drawn = 0;
  for (const Wire &wire : hanging)
    if (hookup(wire.from, wire.out, wire.to, wire.in)) drawn += 1;
  return drawn;
}

void SOUND::GRAPH::shed(const Wire &drawn) {
  ::taken(standing().slack, drawn.from, Side::OUT, drawn.out);
  ::taken(standing().slack, drawn.to, Side::IN, drawn.in);
}

void SOUND::GRAPH::shed(const String &gone) {
  Vector<Wire> kept;
  for (const Wire &wire : standing().slack)
    if (wire.from != gone && wire.to != gone) kept.push_back(wire);
  standing().slack = kept;
}

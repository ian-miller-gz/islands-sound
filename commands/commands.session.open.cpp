// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../render.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../views.hpp"
#include "../commands.hpp"
#include "commands.internal.hpp"

using namespace SOUND;

namespace {

void reread() {
  for (Whole index = 0; index < INVENTORY::held().stocks.size(); ++index) {
    const Stock &stock = INVENTORY::held().stocks[index];
    if (stock.kind != KIND::AUDIO || stock.take.path.empty()) continue;
    INVENTORY::take(index, String(stock.take.path));
  }
}

auto mirrored(const String &name) -> Vector<Whole> {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    if (tracks[track].root == name) return TIMELINE::kinds(track);
  return {};
}

auto stood(const Node &planned) -> String {
  if (planned.seat == Node::ROOT)
    return GRAPH::root(::mirrored(planned.name), planned.name);
  if (planned.seat == Node::RECORD)
    return GRAPH::record(planned.claim, planned.name);
  if (planned.clock) return GRAPH::clock(planned.device, planned.name);
  if (!planned.device.empty())
    return GRAPH::surface(planned.plugin, planned.device, planned.name);
  return GRAPH::seat(planned.plugin, planned.name);
}

void laned(const Node &planned) {
  for (const Port &in : planned.ins)
    if (in.lane != NONE) GRAPH::admit(planned.name, in.lane, true);
  for (Whole out = 0; out < planned.outs.size(); ++out)
    if (planned.outs[out].bypassed) GRAPH::bypass(planned.name, out, true);
}

void restood(SHELL::Session &session, const Graph &plan) {
  for (const Node &planned : plan.nodes) {
    const String node = ::stood(planned);
    if (node.empty()) {
      session.print(std::format("seat refused: {}", planned.name));
      continue;
    }
    if (planned.seat == Node::ROOT) ::laned(planned);
    if (planned.across != NONE) GRAPH::home(node, planned.across, planned.down);
  }
  for (const Node &planned : plan.nodes)
    for (const Berth &berth : planned.berths)
      GRAPH::home(planned.name, berth.page, berth.across, berth.down);
}

void refilled() {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    if (GRAPH::rooted(track.root) && GRAPH::recorder(track.root).empty())
      GRAPH::record(track.root);
}

void redrawn(SHELL::Session &session, const Graph &plan) {
  for (const Wire &wire : plan.wires) {
    const STRING::Hot refused =
      GRAPH::refusal(wire.from, wire.out, wire.to, wire.in);
    if (refused[0] != '\0')
      session.print(std::format(
        "wire refused: {} {} {} {} {}", wire.from, wire.out, wire.to, wire.in,
        refused));
    else
      GRAPH::hookup(wire.from, wire.out, wire.to, wire.in);
  }
}

void reclaimed(const Graph &plan) {
  for (const Node &planned : plan.nodes)
    if (!planned.claim.empty()) GRAPH::claim(planned.name, planned.claim);
}

void remarked(const Graph &plan) {
  for (const Node &planned : plan.nodes) {
    if (planned.quiet) GRAPH::quiet(planned.name, true);
    if (planned.lane != 0) GRAPH::lane(planned.name, planned.lane);
  }
  RENDER::VOICE::drop();
  RENDER::VOICE::claim();
}

void reasserted() {
  for (const Value &value : SESSION::held().values)
    GRAPH::hand(value.address.node, value.address.parameter, value.value);
}

}  // namespace

void SOUND::COMMANDS::stand(SHELL::Session &session, const Graph &plan) {
  ::restood(session, plan);
  ::refilled();
  ::reclaimed(plan);
  ::redrawn(session, plan);
  ::remarked(plan);
}

auto SOUND::COMMANDS::open(const String &name) -> String {
  const STRING::Hot refused = SESSION::open(name);
  if (refused[0] != '\0') return std::format("open refused: {}", refused);
  Gathered told;
  TRANSPORT::adopt(SESSION::held().setting);
  TIMELINE::adopt(SESSION::held().arrangement);
  INVENTORY::adopt(SESSION::held().inventory);
  HISTORY::adopt(SESSION::held().history);
  ::reread();
  GRAPH::close();
  stand(told, SESSION::held().graph);
  CONTROL::adopt(SESSION::held().values);
  ::reasserted();
  if (SESSION::rewritten() != 0)
    told.print(std::format("rewritten from schema {}", SESSION::rewritten()));
  const Vector<Husk> husks = SESSION::husks();
  if (!husks.empty()) told.print(std::format("held {} husks", husks.size()));
  FIELDS::Map face;
  if (SESSION::kept(SESSION::held().name, face)) VIEWS::keep(face);
  told.print(std::format("opened {}", SESSION::held().name));
  String said;
  for (const String &line : told.lines)
    said += (said.empty() ? "" : "\n") + line;
  return said;
}

void SOUND::COMMANDS::open(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("open <name>");
  const String said = open(session.arguments[1]);
  for (Whole from = 0, at = 0; at <= said.size(); ++at)
    if (at == said.size() || said[at] == '\n') {
      session.print(said.substr(from, at - from));
      from = at + 1;
    }
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <format>

#include "../graph.hpp"
#include "../history.hpp"
#include "../kind.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../views.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto valued(const String &token) -> Float {
  return std::strtof(token.c_str(), nullptr);
}

auto spoken(Side side) -> STRING::Hot {
  return side == Side::IN ? "in" : "out";
}

auto spoken(Whole seat) -> STRING::Hot {
  if (seat == Node::ROOT) return "root";
  return seat == Node::RECORD ? "record" : "plugin";
}

auto barred(const Node &node, Side side, Whole port) -> Flag {
  return node.seat == Node::RECORD && side == Side::IN &&
         GRAPH::blocked(node.claim, port);
}

auto crossing(Side side, const Node &node, const Vector<Port> &ports)
  -> String {
  if (ports.empty()) return {};
  String said = std::format(" {}", ::spoken(side));
  for (Whole port = 0; port < ports.size(); ++port)
    said += " " + String(KIND::spoken(ports[port].kind)) +
            (::barred(node, side, port) ? " blocked" : "") +
            (ports[port].bypassed ? " bypassed" : "");
  return said;
}

auto paged(const String &node, const Vector<String> &arguments) -> String {
  if (arguments.size() > 4) return arguments[4];
  if (GRAPH::rooted(node)) return String();
  const String claim = GRAPH::claimed(node);
  return claim.empty() ? VIEWS::WIRED::steering() : claim;
}

auto claimed(const String &name) -> String {
  const String root = GRAPH::claimed(name);
  return root.empty() ? String() : std::format(" claimed {}", root);
}

auto hushed(const String &name) -> String {
  return GRAPH::quieted(name) ? " quiet" : String();
}

auto seated(const String &name) -> String {
  const Whole stood = GRAPH::at(name);
  if (stood == NONE) return std::format("node {} gone", name);
  const Node &node = GRAPH::held().nodes[stood];
  const STRING::Hot seat = ::spoken(node.seat);
  const String seats =
    node.plugin.empty() || node.plugin == node.name ? String() : " " + node.plugin;
  const String device = node.device.empty() ? String() : " on " + node.device;
  return std::format(
    "node {} {}{}{}{}{}{}{}{}", node.name, seat, seats,
    ::crossing(Side::IN, node, node.ins),
    ::crossing(Side::OUT, node, node.outs), ::claimed(name), ::hushed(name),
    device, node.clock ? " clock" : "");
}

auto worded(STRING::Hot word, const Wire &wire) -> String {
  const Node &source = GRAPH::held().nodes[GRAPH::at(wire.from)];
  return std::format(
    "{} {} {} {} {} {}{}", word, wire.from, wire.out, wire.to, wire.in,
    KIND::spoken(source.outs[wire.out].kind),
    GRAPH::dormant(wire) ? " dormant" : "");
}

auto drawn(const Wire &wire) -> String { return ::worded("wire", wire); }

auto covered(const String &node) -> Graph {
  Graph facts = {.nodes = {GRAPH::held().nodes[GRAPH::at(node)]}};
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == node || wire.to == node) facts.wires.push_back(wire);
  return facts;
}

}  // namespace

void SOUND::COMMANDS::offers(SHELL::Session &session) {
  const Vector<String> names = GRAPH::offers();
  session.print(std::format("plugins {}", names.size()));
  for (const String &name : names) session.print(name);
}

void SOUND::COMMANDS::seat(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("seat <plugin> [track]");
  String root;
  if (session.arguments.size() > 2) {
    const Whole track = ::counted(session.arguments[2]);
    if (track >= TIMELINE::held().tracks.size())
      return session.print("seat refused: no such track");
    root = TIMELINE::held().tracks[track].root;
    if (!GRAPH::rooted(root))
      return session.print("seat refused: that track has no root");
  }
  if (GRAPH::bound(session.arguments[1]))
    return session.print(
      "seat refused: a surface seats by its device — "
      "surface <track> <kind> <device ...>");
  const String node = GRAPH::seat(session.arguments[1]);
  if (node.empty()) return session.print("no such plugin");
  SESSION::seated(session.arguments[1]);
  GRAPH::claim(node, root);
  HISTORY::record({.act = "seat", .graph = ::covered(node)});
  session.print(::seated(node));
}

void SOUND::COMMANDS::root(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print(::seated(GRAPH::root({})));
  const Whole track = ::counted(session.arguments[1]);
  if (track >= TIMELINE::held().tracks.size())
    return session.print("root refused: no such track");
  String node = TIMELINE::held().tracks[track].root;
  if (!GRAPH::rooted(node)) {
    node = root(track, "root");
    TIMELINE::root(track, node);
  }
  GRAPH::record(node);
  session.print(::seated(node));
  session.print(std::format("root {} on track {}", node, track));
}

void SOUND::COMMANDS::wire(SHELL::Session &session) {
  if (session.arguments.size() < 5)
    return session.print("wire <from> <out> <to> <in>");
  const String &from = session.arguments[1];
  const Whole out = ::counted(session.arguments[2]);
  const String &to = session.arguments[3];
  const Whole in = ::counted(session.arguments[4]);
  const STRING::Hot refused = GRAPH::refusal(from, out, to, in);
  if (refused[0] != '\0')
    return session.print(std::format("wire refused: {}", refused));
  GRAPH::hookup(from, out, to, in);
  HISTORY::record(
    {.act = "wire", .graph = {.wires = {GRAPH::held().wires.back()}}});
  session.print(::drawn(GRAPH::held().wires.back()));
}

void SOUND::COMMANDS::unwire(SHELL::Session &session) {
  if (session.arguments.size() < 5)
    return session.print("unwire <from> <out> <to> <in>");
  const String &from = session.arguments[1];
  const Whole out = ::counted(session.arguments[2]);
  const String &to = session.arguments[3];
  const Whole in = ::counted(session.arguments[4]);
  if (!GRAPH::unwire(from, out, to, in))
    return session.print("unwire refused: no such wire");
  HISTORY::record(
    {.act = "unwire",
     .verb = Edit::REMOVED,
     .graph = {.wires = {{.from = from, .out = out, .to = to, .in = in}}}});
  session.print(std::format("unwired {} {} {} {}", from, out, to, in));
}

void SOUND::COMMANDS::quiet(SHELL::Session &session) {
  if (session.arguments.size() < 3) return session.print("quiet <node> on|off");
  const String &said = session.arguments[2];
  if (said != "on" && said != "off")
    return session.print("quiet refused: say on or off");
  const String &node = session.arguments[1];
  if (!quieted(node, said == HISTORY::WORD::ON))
    return session.print("quiet refused: no such node, or a root");
  session.print(::seated(node));
}

void SOUND::COMMANDS::unseat(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("unseat <node>");
  const String &node = session.arguments[1];
  if (GRAPH::at(node) == NONE)
    return session.print("unseat refused: no such node");
  if (GRAPH::rooted(node))
    return session.print("unseat refused: a track leaves by its own command");
  if (GRAPH::held().nodes[GRAPH::at(node)].seat == Node::RECORD)
    return session.print(
      "unseat refused: a track's recorder leaves with its track");
  Edit edit = {
    .act = "unseat", .verb = Edit::REMOVED, .graph = ::covered(node)};
  if (!unseated(node, edit.swept)) return session.print("unseat refused");
  HISTORY::record(edit);
  session.print(std::format(
    "unseated {} nodes {} wires {}", node, GRAPH::held().nodes.size(),
    GRAPH::held().wires.size()));
}

void SOUND::COMMANDS::home(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("home <node> <across> <down> [<page>]");
  const String &node = session.arguments[1];
  const String page = ::paged(node, session.arguments);
  if (!homed(
        node, page, ::valued(session.arguments[2]),
        ::valued(session.arguments[3])))
    return session.print("home refused: no such node or page");
  const Berth stood = GRAPH::berth(node, page);
  session.print(std::format(
    "homed {} at {:g} {:g}{}", node, stood.across, stood.down,
    page.empty() ? String() : " on " + page));
}

void SOUND::COMMANDS::wiring(SHELL::Session &session) {
  session.print(std::format(
    "graph {} nodes {} wires", GRAPH::held().nodes.size(),
    GRAPH::held().wires.size()));
  for (const Node &node : GRAPH::held().nodes)
    session.print(::seated(node.name));
  for (const Wire &wire : GRAPH::held().wires) session.print(::drawn(wire));
  for (const Wire &wire : GRAPH::held().slack)
    session.print(::worded("slack", wire));
}

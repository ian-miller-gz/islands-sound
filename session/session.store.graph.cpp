// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto speaks(const String &tag) -> Flag {
  return tag == "s" || tag == "p" || tag == "h" || tag == "k" || tag == "c" ||
         tag == "q" || tag == "d" || tag == "l" || tag == "m" || tag == "f" ||
         tag == "g" || tag == "e" || tag == "a" || tag == "b";
}

auto retired(const String &tag) -> Flag { return tag == "t"; }

auto berthed(std::istringstream &line) -> Berth {
  Berth berth;
  line >> berth.across >> berth.down;
  berth.page = SESSION::rest(line);
  return berth;
}

auto laned(std::istringstream &line) -> Port {
  Port in;
  line >> in.lane;
  return in;
}

void bypassed(std::istringstream &line, Vector<Port> &outs) {
  Whole out = 0;
  line >> out;
  if (out >= outs.size()) outs.resize(out + 1);
  outs[out].bypassed = true;
}

void marked(const String &tag, std::istringstream &line, Node &node) {
  if (tag == "p")
    node.plugin = SESSION::rest(line);
  else if (tag == "h")
    line >> node.across >> node.down;
  else if (tag == "k")
    node.berths.push_back(::berthed(line));
  else if (tag == "c")
    node.claim = SESSION::rest(line);
  else if (tag == "d")
    node.device = SESSION::rest(line);
  else if (tag == "l")
    line >> node.lane;
  else if (tag == "m")
    node.clock = true;
  else if (tag == "a")
    node.ins.push_back(::laned(line));
  else if (tag == "b")
    ::bypassed(line, node.outs);
  else
    node.quiet = true;
}

auto lanes(const Node &node) -> String {
  String text;
  for (const Port &in : node.ins)
    if (in.lane != NONE) text += std::format("a {}\n", in.lane);
  for (Whole out = 0; out < node.outs.size(); ++out)
    if (node.outs[out].bypassed) text += std::format("b {}\n", out);
  return text;
}

}  // namespace

auto SOUND::SESSION::written(const Graph &wiring) -> String {
  String text;
  for (const Node &node : wiring.nodes) {
    text += std::format("s {} {}\n", node.seat, node.name);
    if (!node.plugin.empty()) text += std::format("p {}\n", node.plugin);
    if (node.across != NONE)
      text += std::format("h {} {}\n", node.across, node.down);
    for (const Berth &berth : node.berths)
      if (berth.across != NONE)
        text +=
          std::format("k {} {} {}\n", berth.across, berth.down, berth.page);
    if (!node.claim.empty()) text += std::format("c {}\n", node.claim);
    if (node.quiet) text += "q\n";
    if (!node.device.empty()) text += std::format("d {}\n", node.device);
    if (node.lane != 0) text += std::format("l {}\n", node.lane);
    if (node.clock) text += "m\n";
    text += ::lanes(node);
  }
  for (const Wire &wire : wiring.wires)
    text +=
      std::format("f {}\ne {} {} {}\n", wire.from, wire.out, wire.in, wire.to);
  for (const Wire &wire : wiring.slack)
    text +=
      std::format("g {}\ne {} {} {}\n", wire.from, wire.out, wire.in, wire.to);
  return text;
}

void SOUND::SESSION::taken(
  const String &text, Graph &wiring, Vector<Husk> &husks) {
  std::istringstream lines(text);
  String from;
  Flag slack = false;
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "s") {
      Node node;
      line >> node.seat;
      node.name = rest(line);
      wiring.nodes.push_back(node);
    } else if (::retired(tag)) {
      continue;
    } else if (!::speaks(tag)) {
      husked(held, at, husks);
    } else if (tag == "f" || tag == "g") {
      from = rest(line);
      slack = tag == "g";
    } else if (tag == "e") {
      Wire wire = {.from = from};
      line >> wire.out >> wire.in;
      wire.to = rest(line);
      (slack ? wiring.slack : wiring.wires).push_back(wire);
    } else if (!wiring.nodes.empty()) {
      ::marked(tag, line, wiring.nodes.back());
    }
  }
}

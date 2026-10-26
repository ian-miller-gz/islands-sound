// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "graph";

auto homed(const Node &node) -> String {
  const Berth held = GRAPH::berth(node.name, VIEWS::WIRED::steering());
  if (held.across == Float(NONE) || held.down == Float(NONE)) return "none";
  return std::format("{:g} {:g}", held.across, held.down);
}

auto sited(const VIEWS::WIRED::Seen &seen) -> String {
  if (!seen.stands) return "stands no";
  if (seen.branch == NONE) return "stands yes";
  return std::format("stands yes branch {} depth {}", seen.branch, seen.depth);
}

void state(SHELL::Session &session) {
  const GUI::Handle page = VIEWS::document();
  const Vector<Node> &nodes = GRAPH::held().nodes;
  const Vector<VIEWS::WIRED::Seen> seen = VIEWS::WIRED::picture();
  Whole standing = 0;
  for (const VIEWS::WIRED::Seen &one : seen) standing += one.stands ? 1 : 0;
  const Whole chose =
    GUI::SAC::SEAT::selected(page, VIEWS::WIRED::board().c_str());
  const STRING::Cold said = GUI::GET::text(page, VIEWS::WIRED::NOTICE);
  session.print(std::format(
    "graph boxes {} standing {} wires {} chosen {} zoom {} notice {}",
    nodes.size(), standing, GRAPH::held().wires.size(),
    chose == NONE ? String("none") : std::to_string(chose),
    GUI::NGA::GET::zoom(page, VIEWS::WIRED::board().c_str()).value,
    said.empty() ? "-" : said));
  for (Whole index = 0; index < nodes.size(); ++index)
    session.print(std::format(
      "box {} {} ins {} outs {} home {} {}", index,
      VIEWS::named(nodes[index].name), nodes[index].ins.size(),
      nodes[index].outs.size(), ::homed(nodes[index]), ::sited(seen[index])));
  VIEWS::WIRED::say(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = ::NAME,
   .least = VIEWS::WIRED::least,
   .controls = {VIEWS::ZOOM, VIEWS::PAN},
   .run = VIEWS::WIRED::draw,
   .state = ::state});

}  // namespace

auto SOUND::VIEWS::WIRED::board() -> String {
  return String(::NAME) + "." + BOARD;
}

auto SOUND::VIEWS::WIRED::socket(
  const String &box, STRING::Hot side, Whole port) -> String {
  return std::format("{}.{}{}", box, side, port);
}

auto SOUND::VIEWS::WIRED::knob(const String &box, Whole out) -> String {
  return std::format("{}.{}{}.{}", box, OUT, out, BYPASS);
}

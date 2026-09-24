// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../commands.hpp"
#include "../../graph.hpp"
#include "../../history.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ADRIFT = "that wire lands on nothing the graph holds";
constexpr STRING::Hot MISSING = "the graph holds no such wire";
constexpr STRING::Hot CIRCLED =
  "closes a circle: dormant until a wire in it is cut";

struct End {
  String node;
  Whole port = NONE;
};

auto ended(STRING::Cold id, STRING::Hot side) -> End {
  const String worn(id);
  const auto dot = worn.rfind('.');
  if (dot == String::npos) return {};
  const String tail = worn.substr(dot + 1);
  const String faced(side);
  if (!tail.starts_with(faced)) return {};
  const Whole row = GUI::SAC::SEAT::row(worn, VIEWS::WIRED::board().c_str());
  if (row >= GRAPH::held().nodes.size()) return {};
  return {
    GRAPH::held().nodes[row].name,
    Whole(std::strtoul(tail.c_str() + faced.size(), nullptr, 10))};
}

void said(GUI::Handle page, STRING::Hot text) {
  GUI::set(page, VIEWS::WIRED::NOTICE, GUI::Text{text});
}

void made(GUI::Handle page, const GUI::NGA::Link &link) {
  const End from = ::ended(link.from, VIEWS::WIRED::OUT);
  const End to = ::ended(link.to, VIEWS::WIRED::IN);
  if (from.node.empty() || to.node.empty()) return ::said(page, ADRIFT);
  const STRING::Hot refused =
    GRAPH::refusal(from.node, from.port, to.node, to.port);
  if (refused[0] != '\0') return ::said(page, refused);
  GRAPH::hookup(from.node, from.port, to.node, to.port);
  HISTORY::record(
    {.act = "wire", .graph = {.wires = {GRAPH::held().wires.back()}}});
  const Wire drawn = {from.node, from.port, to.node, to.port};
  ::said(page, GRAPH::dormant(drawn) ? CIRCLED : "");
}

void parted(GUI::Handle page, const GUI::NGA::Link &link) {
  const End from = ::ended(link.from, VIEWS::WIRED::OUT);
  const End to = ::ended(link.to, VIEWS::WIRED::IN);
  if (from.node.empty() || to.node.empty()) return ::said(page, ADRIFT);
  if (!GRAPH::unwire(from.node, from.port, to.node, to.port))
    return ::said(page, MISSING);
  HISTORY::record(
    {.act = "unwire",
     .verb = Edit::REMOVED,
     .graph = {
       .wires = {
         {.from = from.node,
          .out = from.port,
          .to = to.node,
          .in = to.port}}}});
  ::said(page, "");
}

}  // namespace

auto SOUND::VIEWS::WIRED::hands() -> Flag {
  const GUI::Handle page = document();
  const Vector<GUI::NGA::Link> drawn = GUI::NGA::GET::joined(page);
  const Vector<GUI::NGA::Link> cut = GUI::NGA::GET::cut(page);
  for (const GUI::NGA::Link &link : drawn) ::made(page, link);
  for (const GUI::NGA::Link &link : cut) ::parted(page, link);
  carried(page);
  const Flag clicked = pinned();
  return !drawn.empty() || !cut.empty() || clicked;
}

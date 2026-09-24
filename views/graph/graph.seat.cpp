// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../graph.hpp"
#include "../../kind.hpp"
#include "graph.browse.internal.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::WIRED::CELL;
using VIEWS::WIRED::deep;
using VIEWS::WIRED::WIDE;

constexpr Float TALL = 44.0f;
constexpr Float LETTER = 14.0f;
constexpr Float AIR = 10.0f;
constexpr Float ORIGIN = 0.0f;
constexpr Float HALVED = 2.0f;
constexpr STRING::Hot CARRY = "carry";
constexpr STRING::Hot RESTING = "○";
constexpr STRING::Hot LANDED = "●";
constexpr STRING::Hot SLACK = "◌";
constexpr STRING::Hot BLOCKED = "⊘";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot DORMANT = "dormant";
constexpr Float DOTTED = 2.0f;
constexpr Float FACED = 3.0f;
constexpr STRING::Hot OPENS = "▤";
constexpr Float RAISED = 1.0f;
constexpr Whole DEVISED = 1;
constexpr Float SMALL = 12.0f;
constexpr Float KNOB = 18.0f;
constexpr Float HALF = ::CELL / 2.0f;
constexpr STRING::Hot PLAYING = "▶";
constexpr STRING::Hot BYPASSED = "▷";
constexpr STRING::Hot HINTED = "hint";
constexpr STRING::Hot HINT =
  "Bypass - silences this lane's clips at the out; what its in admits still "
  "plays, and the lane may record.";

constexpr STRING::Hot TYPED = "node";
constexpr STRING::Hot WORDS[] = {
  "instrument", "effect", "shaper", "controller", "plug"};
static_assert(std::size(WORDS) == VIEWS::WIRED::BROWSE::PLUGS + 1);
constexpr STRING::Hot ROOTED = "root";
constexpr STRING::Hot TAKING = "record";
constexpr STRING::Hot TICKING = "clock";

auto dressed(const Node &node) -> String {
  const STRING::Hot word =
    node.seat == Node::ROOT     ? ::ROOTED
    : node.seat == Node::RECORD ? ::TAKING
    : node.clock                ? ::TICKING
                                : ::WORDS[VIEWS::WIRED::BROWSE::shelved(
                     GRAPH::takes(node.plug), GRAPH::gives(node.plug))];
  return String(::TYPED) + word;
}

auto surfaced(const Node &node) -> Flag {
  return node.seat == Node::PLUG && GRAPH::bound(node.plug);
}

auto dotted(const Node &node) -> Flag {
  return node.seat != Node::ROOT && node.seat != Node::RECORD;
}

Whole shown = 0, drawn = 0, stirred = 0;
String pictured;
Whole born = 0;

auto labelled() -> Float {
  return (::WIDE - ::CELL * ::HALVED - ::AIR) / ::HALVED;
}

void knob(const String &box, Whole out, Whole shift) {
  const GUI::Handle page = VIEWS::document();
  const String id = VIEWS::WIRED::knob(box, out);
  const Float down = ::CELL * Float(1 + shift + out);
  BOARDS::place(
    page, box.c_str(), "button", id, {::WIDE - ::HALF - ::KNOB, down},
    {::KNOB, ::LETTER});
  GUI::set(page, id.c_str(), GUI::Style{BOARDS::PLATE});
  GUI::set(page, id.c_str(), GUI::Size{::SMALL});
  const String hint = id + "." + ::HINTED;
  GUI::NODES::create(page, id.c_str(), "label", hint.c_str());
  GUI::set(page, hint.c_str(), GUI::Visibility{false});
  GUI::set(page, hint.c_str(), GUI::Text{::HINT});
}

void block(
  const String &box, STRING::Hot side, GUI::NGA::Port::Side facing, Whole port,
  const Port &worn, Float x, Whole shift, Flag muted, Flag knobbed) {
  const GUI::Handle page = VIEWS::document();
  const String id = VIEWS::WIRED::socket(box, side, port);
  const String dress = muted ? String(VIEWS::WIRED::QUIET)
                             : String(::CARRY) + KIND::spoken(worn.kind);
  const Float down = ::CELL * Float(1 + shift + port);
  const Float cell = facing == GUI::NGA::Port::IN ? ::CELL : ::HALF;
  BOARDS::place(page, box.c_str(), "label", id, {x, down}, {cell, ::CELL});
  GUI::set(page, id.c_str(), GUI::Style{dress.c_str()});
  GUI::set(page, id.c_str(), GUI::Size{::LETTER});
  GUI::set(page, id.c_str(), GUI::Text{::RESTING});
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Port{facing, VIEWS::WIRED::worded(worn).c_str(), muted});
  const Float wide = ::labelled() - (knobbed ? ::KNOB - ::AIR : 0.0f);
  const Float band = facing == GUI::NGA::Port::IN
                       ? ::CELL
                       : ::WIDE - ::HALF - wide - (knobbed ? ::KNOB : 0.0f);
  VIEWS::WIRED::label(box, id, {band, down}, {wide, ::CELL}, ::LETTER, worn);
}

auto banded() -> GUI::Extent {
  return {::WIDE - ::CELL * ::FACED - ::AIR, ::CELL};
}

void dot(const String &box) {
  const GUI::Handle page = VIEWS::document();
  const String id = box + "." + VIEWS::WIRED::QUIET;
  BOARDS::place(
    page, box.c_str(), "button", id, {::WIDE - ::CELL * ::DOTTED, ::ORIGIN},
    {::CELL, ::CELL});
  GUI::set(page, id.c_str(), GUI::Style{BOARDS::PLATE});
  GUI::set(page, id.c_str(), GUI::Size{::LETTER});
}

void face(const String &box) {
  const GUI::Handle page = VIEWS::document();
  const String id = box + "." + VIEWS::WIRED::FACE;
  BOARDS::place(
    page, box.c_str(), "button", id, {::WIDE - ::CELL * ::FACED, ::ORIGIN},
    {::CELL, ::CELL});
  GUI::set(page, id.c_str(), GUI::Style{BOARDS::PLATE});
  GUI::set(page, id.c_str(), GUI::Size{::LETTER});
  GUI::set(page, id.c_str(), GUI::Text{::OPENS});
}

void box(Whole index) {
  const GUI::Handle page = VIEWS::document();
  const Node &node = GRAPH::held().nodes[index];
  const String id = GUI::SAC::SEAT::named(VIEWS::WIRED::board().c_str(), index);
  BOARDS::place(
    page, VIEWS::WIRED::board().c_str(), "node", id, {},
    {::WIDE, ::deep(node)});
  GUI::set(page, id.c_str(), GUI::Style{::dressed(node).c_str()});
  GUI::set(page, id.c_str(), GUI::Clipping{true});
  GUI::set(page, id.c_str(), GUI::Size{::LETTER});
  GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  VIEWS::WIRED::title(id, {::AIR, ::ORIGIN}, ::banded(), ::LETTER);
  const Whole shift = ::surfaced(node) ? ::DEVISED : 0;
  if (shift != 0)
    VIEWS::WIRED::subtitle(
      id, {::AIR, ::CELL}, {::WIDE - ::AIR * ::HALVED, ::CELL}, ::SMALL);
  const String steered = VIEWS::WIRED::steering();
  const Flag foreign =
    node.seat == Node::ROOT && !steered.empty() && node.name != steered;
  for (Whole port = 0; port < node.ins.size(); ++port)
    ::block(
      id, VIEWS::WIRED::IN, GUI::NGA::Port::IN, port, node.ins[port], ::ORIGIN,
      shift, false, false);
  for (Whole port = 0; port < node.outs.size(); ++port) {
    ::block(
      id, VIEWS::WIRED::OUT, GUI::NGA::Port::OUT, port, node.outs[port],
      ::WIDE - ::HALF, shift, foreign, node.seat == Node::ROOT);
    if (node.seat == Node::ROOT && GRAPH::admitted(node.name, port) != NONE)
      ::knob(id, port, shift);
  }
  if (::dotted(node)) ::dot(id);
  if (GRAPH::offered(node.name) != nullptr) ::face(id);
}

void titled(Whole index, const String &box) {
  const GUI::Handle page = VIEWS::document();
  const Node &node = GRAPH::held().nodes[index];
  const GUI::Extent band = ::banded();
  GUI::set(
    page, (box + "." + VIEWS::WIRED::NAME).c_str(),
    GUI::Text{VIEWS::WIRED::cropped(VIEWS::named(node.name), band.w, ::LETTER)
                .c_str()});
  if (!::surfaced(node)) return;
  GUI::set(
    page, (box + "." + VIEWS::WIRED::DEVICE).c_str(),
    GUI::Text{VIEWS::WIRED::cropped(
                VIEWS::device(node.name), ::WIDE - ::AIR * ::HALVED, ::SMALL)
                .c_str()});
}

void worn(Whole index, const String &box) {
  const GUI::Handle page = VIEWS::document();
  const Flag quiet = GRAPH::held().nodes[index].quiet;
  const Flag chosen = GUI::NGA::GET::selected(page, box.c_str());
  const String dress = ::dressed(GRAPH::held().nodes[index]);
  GUI::set(
    page, box.c_str(),
    GUI::Style{
      quiet    ? VIEWS::WIRED::QUIET
      : chosen ? ::MARKED
               : dress.c_str()});
  if (!::dotted(GRAPH::held().nodes[index])) return;
  const String id = box + "." + VIEWS::WIRED::QUIET;
  GUI::set(page, id.c_str(), GUI::Text{quiet ? ::RESTING : ::LANDED});
}

auto marked(const Node &node, Side side, Whole port) -> STRING::Hot {
  if (GRAPH::joined(node.name, side, port) > 0) return ::LANDED;
  if (GRAPH::slackened(node.name, side, port) > 0) return ::SLACK;
  const Flag taking = node.seat == Node::RECORD && side == Side::IN;
  return taking && GRAPH::blocked(node.claim, port) ? ::BLOCKED : ::RESTING;
}

void knobbed(Whole index, const String &box) {
  const Node &node = GRAPH::held().nodes[index];
  if (node.seat != Node::ROOT) return;
  const GUI::Handle page = VIEWS::document();
  for (Whole out = 0; out < node.outs.size(); ++out)
    if (GRAPH::admitted(node.name, out) != NONE)
      GUI::set(
        page, VIEWS::WIRED::knob(box, out).c_str(),
        GUI::Text{node.outs[out].bypassed ? ::BYPASSED : ::PLAYING});
}

void barred(Whole index, const String &box) {
  const Node &node = GRAPH::held().nodes[index];
  const Flag taking = node.seat == Node::RECORD;
  if (!taking && node.seat != Node::ROOT) return;
  const Side side = taking ? Side::IN : Side::OUT;
  const STRING::Hot faced = taking ? VIEWS::WIRED::IN : VIEWS::WIRED::OUT;
  const Vector<Port> &ports = taking ? node.ins : node.outs;
  const GUI::Handle page = VIEWS::document();
  for (Whole port = 0; port < ports.size(); ++port)
    GUI::set(
      page, VIEWS::WIRED::socket(box, faced, port).c_str(),
      GUI::Text{::marked(node, side, port)});
}

void hung(
  GUI::Handle page, const String &board, Whole row, const String &node,
  Side side, Whole port) {
  if (GRAPH::joined(node, side, port) > 0) return;
  const String id = VIEWS::WIRED::socket(
    GUI::SAC::SEAT::named(board.c_str(), row),
    side == Side::OUT ? VIEWS::WIRED::OUT : VIEWS::WIRED::IN, port);
  GUI::set(page, id.c_str(), GUI::Text{::SLACK});
}

void build(Whole nodes, Whole wires, const Vector<VIEWS::WIRED::Seen> &seen) {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::WIRED::board();
  BOARDS::sweep(page, board.c_str(), 0, std::max(::shown, nodes));
  for (Whole index = 0; index < nodes; ++index)
    if (seen[index].stands) ::box(index);
  for (const Wire &wire : GRAPH::held().wires) {
    const Whole source = GRAPH::at(wire.from), sink = GRAPH::at(wire.to);
    if (source == NONE || sink == NONE) continue;
    if (!seen[source].stands || !seen[sink].stands) continue;
    const String from = VIEWS::WIRED::socket(
      GUI::SAC::SEAT::named(board.c_str(), source), VIEWS::WIRED::OUT,
      wire.out);
    const String to = VIEWS::WIRED::socket(
      GUI::SAC::SEAT::named(board.c_str(), sink), VIEWS::WIRED::IN, wire.in);
    GUI::NGA::connect(
      page, from.c_str(), to.c_str(),
      GRAPH::dormant(wire) ? GUI::Style{::DORMANT} : GUI::Style{});
    GUI::set(page, from.c_str(), GUI::Text{::LANDED});
    GUI::set(page, to.c_str(), GUI::Text{::LANDED});
  }
  for (const Wire &wire : GRAPH::held().slack) {
    const Whole source = GRAPH::at(wire.from), sink = GRAPH::at(wire.to);
    if (source == NONE || sink == NONE) continue;
    if (!seen[source].stands || !seen[sink].stands) continue;
    ::hung(page, board, source, wire.from, Side::OUT, wire.out);
    ::hung(page, board, sink, wire.to, Side::IN, wire.in);
  }
  ::shown = nodes;
  ::drawn = wires;
}

}  // namespace

auto SOUND::VIEWS::WIRED::deep(const Node &node) -> Float {
  const Whole rows = std::max(node.ins.size(), node.outs.size()) + 1 +
                     (::surfaced(node) ? ::DEVISED : 0);
  return std::max(::TALL, ::CELL * Float(rows));
}

void SOUND::VIEWS::WIRED::draw() {
  const GUI::Handle page = document();
  browse();
  door();
  doors();
  menu();
  acts();
  const Flag again = hands();
  const Whole nodes = GRAPH::held().nodes.size();
  const Whole wires = GRAPH::held().wires.size();
  const Vector<Seen> seen = picture();
  const String track = steering();
  const Flag renewed = VIEWS::reborn(::born);
  const Whole stirred = GRAPH::counted();
  const Flag reseated = again || nodes != ::shown || wires != ::drawn ||
                        track != ::pictured || stirred != ::stirred;
  const Flag seated = reseated || renewed;
  if (seated) ::build(nodes, wires, seen);
  ::pictured = track;
  ::stirred = stirred;
  const GUI::Extent view = GUI::GET::measured(page, board().c_str());
  for (Whole index = 0; index < nodes; ++index) {
    if (!seen[index].stands) continue;
    const String id = GUI::SAC::SEAT::named(board().c_str(), index);
    GUI::set(page, id.c_str(), placed(index, seen, view));
    ::titled(index, id);
    ::worn(index, id);
    ::barred(index, id);
    ::knobbed(index, id);
  }
  bound();
  follow(reseated);
  grid();
  aim();
}

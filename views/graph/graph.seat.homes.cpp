// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::WIRED::CELL;
using VIEWS::WIRED::deep;
using VIEWS::WIRED::Seen;
using VIEWS::WIRED::WIDE;

constexpr Float SPAN = ::WIDE + ::CELL * 2.0f;
constexpr Float DROP = 90.0f;
constexpr Whole ACROSS = 3;
constexpr Float ORIGIN = 0.0f;
constexpr Float HALVED = 2.0f;

auto before(Whole index, const Vector<Seen> &seen) -> Whole {
  for (Whole step = index; step > 0; --step)
    if (seen[step - 1].stands) return step - 1;
  return NONE;
}

auto crowded(
  const GUI::Position &at, Float tall, const Vector<Seen> &seen,
  const String &page) -> Float {
  const Vector<Node> &nodes = GRAPH::held().nodes;
  Float under = Float(NONE);
  for (Whole box = 0; box < nodes.size(); ++box) {
    const Berth berth = GRAPH::berth(nodes[box].name, page);
    if (!seen[box].stands || berth.across == Float(NONE)) continue;
    const Float foot = berth.down + ::deep(nodes[box]);
    if (at.x >= berth.across + ::WIDE || berth.across >= at.x + ::WIDE)
      continue;
    if (at.y >= foot || berth.down >= at.y + tall) continue;
    under = under == Float(NONE) ? foot : std::max(under, foot);
  }
  return under;
}

auto cleared(
  GUI::Position at, Float tall, const Vector<Seen> &seen, const String &page,
  Float top, Float foot) -> GUI::Position {
  for (Whole step = 0; step <= seen.size(); ++step) {
    if (at.y + tall > foot) at = {at.x + ::SPAN, top + ::CELL};
    const Float under = ::crowded(at, tall, seen, page);
    if (under == Float(NONE)) return at;
    at = {at.x, under + ::CELL};
  }
  return at;
}

auto stacked(const Berth &held, const Node &prior) -> GUI::Position {
  return {held.across, held.down + ::deep(prior) + ::CELL};
}

auto derived(
  Whole index, const Vector<Seen> &seen, const GUI::Extent &view,
  const String &page) -> GUI::Position {
  const GUI::Handle document = VIEWS::document();
  const String board = VIEWS::WIRED::board();
  const Float scale = GUI::NGA::GET::zoom(document, board.c_str()).value;
  if (view.w <= ::ORIGIN || view.h <= ::ORIGIN || scale <= ::ORIGIN)
    return {
      ::CELL + Float(index % ::ACROSS) * ::SPAN,
      ::CELL + Float(index / ::ACROSS) * ::DROP};
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(document, board.c_str());
  const Float foot = pan.y + view.h / scale;
  const GUI::Position middle = {
    pan.x + view.w / scale / ::HALVED, (pan.y + foot) / ::HALVED};
  const Whole prior = ::before(index, seen);
  const Vector<Node> &nodes = GRAPH::held().nodes;
  const GUI::Position at =
    prior == NONE
      ? middle
      : ::stacked(GRAPH::berth(nodes[prior].name, page), nodes[prior]);
  const GUI::Position clear =
    ::cleared(at, ::deep(nodes[index]), seen, page, pan.y, foot);
  return {std::max(::ORIGIN, clear.x), std::max(::ORIGIN, clear.y)};
}

}  // namespace

auto SOUND::VIEWS::WIRED::placed(
  Whole index, const Vector<Seen> &seen,
  const GUI::Extent &view) -> GUI::Position {
  const Node &node = GRAPH::held().nodes[index];
  const String page = VIEWS::WIRED::steering();
  const Berth held = GRAPH::berth(node.name, page);
  if (held.across != Float(NONE) && held.down != Float(NONE))
    return {held.across, held.down};
  const GUI::Position at = ::derived(index, seen, view, page);
  GRAPH::home(node.name, page, at.x, at.y);
  return at;
}

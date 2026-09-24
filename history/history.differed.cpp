// SPDX-License-Identifier: AGPL-3.0-or-later
#include "history.internal.hpp"

namespace {
using namespace SOUND;

auto same(const Note &one, const Note &other) -> Flag {
  return one.pitch == other.pitch && one.at == other.at &&
         one.length == other.length && one.velocity == other.velocity;
}

auto same(const Turn &one, const Turn &other) -> Flag {
  return one.at == other.at && one.number == other.number &&
         one.value == other.value && one.shape == other.shape;
}

auto same(const Point &one, const Point &other) -> Flag {
  return one.at == other.at && one.value == other.value &&
         one.shape == other.shape;
}

auto same(const Choice &one, const Choice &other) -> Flag {
  return one.at == other.at && one.program == other.program;
}

auto paired(const Note &one, const Note &other) -> Flag {
  return one.pitch == other.pitch;
}

auto paired(const Turn &one, const Turn &other) -> Flag {
  return one.number == other.number;
}

auto paired(const Point &, const Point &) -> Flag { return true; }

auto paired(const Choice &, const Choice &) -> Flag { return true; }

template <typename Row, typename Lay>
void crossed(
  const Row &stood, const Row &stands, Whole was, Whole now, Lay lay) {
  if (was == now && ::paired(stood, stands)) return lay(was, &stood, &stands);
  lay(was, &stood, nullptr);
  lay(now, nullptr, &stands);
}

template <typename Row, typename Lay>
void walked(const Vector<Row> &stood, const Vector<Row> &stands, Lay lay) {
  Whole was = 0, now = 0;
  while (was < stood.size() && now < stands.size()) {
    if (::same(stood[was], stands[now])) {
      ++was, ++now;
    } else if (stood[was].at < stands[now].at) {
      lay(was, &stood[was], nullptr);
      ++was;
    } else if (stands[now].at < stood[was].at) {
      lay(now, nullptr, &stands[now]);
      ++now;
    } else {
      ::crossed(stood[was], stands[now], was, now, lay);
      ++was, ++now;
    }
  }
  for (; was < stood.size(); ++was) lay(was, &stood[was], nullptr);
  for (; now < stands.size(); ++now) lay(now, nullptr, &stands[now]);
}

auto plotted(const Turn &node) -> Point {
  return {.at = node.at, .value = node.value, .shape = node.shape};
}

auto plotted(const Point &node) -> Point { return node; }

auto plotted(const Choice &node) -> Point {
  return {.at = node.at, .value = Float(node.program)};
}

auto numbered(const Turn &node) -> Whole { return node.number; }

auto numbered(const Point &) -> Whole { return 0; }

auto numbered(const Choice &) -> Whole { return 0; }

template <typename Row>
void laid(
  Whole stock, const Vector<Row> &stood, const Vector<Row> &stands,
  Vector<Plotted> &rows) {
  ::walked(stood, stands, [&](Whole at, const Row *was, const Row *now) {
    Plotted one = {.stock = stock, .index = at};
    if (was != nullptr) one.was = ::plotted(*was);
    if (now != nullptr) one.point = ::plotted(*now);
    one.number = ::numbered(now == nullptr ? *was : *now);
    rows.push_back(one);
  });
}

}  // namespace

void SOUND::HISTORY::differed(
  Whole stock, const Stock &stood, const Stock &stands, Vector<Noted> &notes,
  Vector<Plotted> &plots) {
  ::walked(
    stood.score.notes, stands.score.notes,
    [&](Whole at, const Note *was, const Note *now) {
      Noted one = {.stock = stock, .index = at};
      if (was != nullptr) one.was = *was;
      if (now != nullptr) one.note = *now;
      notes.push_back(one);
    });
  ::laid(stock, stood.curve.points, stands.curve.points, plots);
  ::laid(stock, stood.turns, stands.turns, plots);
  ::laid(stock, stood.choices, stands.choices, plots);
}

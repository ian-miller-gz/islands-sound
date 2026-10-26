// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include "../../shape.hpp"
#include "signals.bend.hpp"

namespace {
using namespace SOUND;

constexpr GUI::Shape::Run RUNS[] = {
  GUI::Shape::HOLD, GUI::Shape::LINE, GUI::Shape::RISE, GUI::Shape::FALL,
  GUI::Shape::EASE};

auto run(const VIEWS::SIGNALS::Point &point) -> Float {
  return point.until > point.at ? VIEWS::SIGNALS::across(point.until - point.at)
                                : 0.0f;
}

auto drop(const Vector<VIEWS::SIGNALS::Point> &rows, Whole row) -> Float {
  const Whole next = VIEWS::SIGNALS::closed(rows, row);
  if (next == NONE) return VIEWS::SIGNALS::ORIGIN;
  return VIEWS::SIGNALS::down(rows[next].value) -
         VIEWS::SIGNALS::down(rows[row].value);
}

auto folded(const VIEWS::SIGNALS::Point &point) -> Flag {
  return point.high > point.low;
}

auto placed(const VIEWS::SIGNALS::Point &point) -> GUI::Position {
  return {
    VIEWS::SIGNALS::across(point.at),
    VIEWS::SIGNALS::down(::folded(point) ? point.high : point.value)};
}

auto ends(const Vector<VIEWS::SIGNALS::Point> &rows, Whole row) -> GUI::Ends {
  const VIEWS::SIGNALS::Point &point = rows[row];
  if (!::folded(point)) return {{::run(point), ::drop(rows, row)}};
  return {
    {VIEWS::SIGNALS::ORIGIN,
     VIEWS::SIGNALS::down(point.low) - VIEWS::SIGNALS::down(point.high)}};
}

auto shaped(const VIEWS::SIGNALS::Point &point) -> GUI::Shape {
  if (::folded(point) || point.shape >= std::size(::RUNS))
    return {GUI::Shape::LINE};
  return {::RUNS[point.shape]};
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::closed(const Vector<Point> &rows, Whole row)
  -> Whole {
  const Whole next = row + 1;
  if (rows[row].shape == SHAPE::HOLD || next >= rows.size()) return NONE;
  if (::folded(rows[row])) return NONE;
  if (rows[next].stock != rows[row].stock || rows[next].at != rows[row].until)
    return NONE;
  return next;
}

void SOUND::VIEWS::SIGNALS::traced(
  GUI::Handle page, const String &id, const Vector<Point> &rows, Whole row) {
  GUI::set(page, id.c_str(), ::placed(rows[row]));
  GUI::set(page, id.c_str(), ::ends(rows, row));
  GUI::set(page, id.c_str(), ::shaped(rows[row]));
  const Flag bowing = closed(rows, row) != NONE;
  GUI::set(page, id.c_str(), GUI::Bend{bowing ? bowed(row) : 0.0f});
}

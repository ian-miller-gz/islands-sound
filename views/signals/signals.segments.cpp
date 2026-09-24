// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../../boards.hpp"
#include "../../shape.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STROKE = "stroke";
constexpr STRING::Hot TREADED = "wash";
constexpr STRING::Hot STEM = "segment";

constexpr GUI::Shape::Run RUNS[] = {
  GUI::Shape::HOLD, GUI::Shape::LINE, GUI::Shape::RISE, GUI::Shape::FALL,
  GUI::Shape::EASE};

VIEWS::Pool pool;

auto segment(Whole row) -> String {
  return std::format("{}.{}.{}", VIEWS::SIGNALS::board(), ::STEM, row);
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = count; row < ::pool.stood; ++row)
    BOARDS::drop(page, ::segment(row));
  for (Whole row = ::pool.stood; row < count; ++row) {
    const String id = ::segment(row);
    BOARDS::place(page, VIEWS::SIGNALS::board().c_str(), ::STROKE, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::SIGNALS::LAID});
  }
  VIEWS::built(::pool, count);
}

auto ran(Whole shape) -> GUI::Shape::Run {
  return shape < std::size(::RUNS) ? ::RUNS[shape] : GUI::Shape::LINE;
}

auto run(const VIEWS::SIGNALS::Point &point) -> Float {
  return point.until > point.at ? VIEWS::SIGNALS::across(point.until - point.at)
                                : 0.0f;
}

auto drop(const Vector<VIEWS::SIGNALS::Point> &rows, Whole row) -> Float {
  const Whole next = row + 1;
  if (rows[row].shape == SHAPE::HOLD || next >= rows.size())
    return VIEWS::SIGNALS::ORIGIN;
  if (rows[next].stock != rows[row].stock || rows[next].at != rows[row].until)
    return VIEWS::SIGNALS::ORIGIN;
  return VIEWS::SIGNALS::down(rows[next].value) -
         VIEWS::SIGNALS::down(rows[row].value);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::SHED::segments() {
  VIEWS::reborn(::pool);
  ::build(0);
}

void SOUND::VIEWS::SIGNALS::segments() {
  const GUI::Handle page = document();
  const Vector<Point> &rows = shown();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, rows.size(), window());
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::segment, rows.size());
  const String skin = dressed(::TREADED);
  for (Whole row = 0; row < rows.size(); ++row) {
    const String id = ::segment(row);
    GUI::set(
      page, id.c_str(),
      GUI::Position{across(rows[row].at), down(rows[row].value)});
    GUI::set(
      page, id.c_str(), GUI::Ends{{::run(rows[row]), ::drop(rows, row)}});
    GUI::set(page, id.c_str(), GUI::Shape{::ran(rows[row].shape)});
    GUI::set(page, id.c_str(), GUI::Style{skin.c_str()});
    GUI::set(page, id.c_str(), GUI::Border{TREAD});
  }
}

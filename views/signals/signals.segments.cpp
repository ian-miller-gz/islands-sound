// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "../../shape.hpp"
#include "signals.bend.hpp"
#include "signals.overridden.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STROKE = "stroke";
constexpr STRING::Hot TREADED = "wash";
constexpr STRING::Hot STEM = "segment";

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

auto bent(Whole row) -> String {
  const Float bend = VIEWS::SIGNALS::bowed(row);
  return bend == 0.0f ? String() : std::format(" bend {:g}", bend);
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
  const String skin = worn(::TREADED);
  for (Whole row = 0; row < rows.size(); ++row) {
    const String id = ::segment(row);
    traced(page, id, rows, row);
    GUI::set(page, id.c_str(), GUI::Style{skin.c_str()});
    GUI::set(page, id.c_str(), GUI::Border{TREAD});
  }
}

void SOUND::VIEWS::SIGNALS::segments(SHELL::Session &session) {
  const Vector<Point> &rows = shown();
  for (Whole row = 0; row < rows.size(); ++row)
    session.print(std::format(
      "segment {} at {} until {} shape {}{}", row, rows[row].at,
      rows[row].until, SHAPE::spoken(rows[row].shape), ::bent(row)));
}

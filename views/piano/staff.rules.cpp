// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/rows.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.paper.hpp"
#include "piano.staff.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr STRING::Hot STEM = "staff";
constexpr STRING::Hot INK = "staffline";
constexpr Float ORIGIN = 0.0f;
constexpr Float SPARE = 0.5f;

Vector<Integer> drawn;
Vector<Float> kept;
Whole born = 0;
Float reach = ::ORIGIN;

auto centre(Integer place) -> Float {
  return (Float(place - STAFF::TOP) + ROLL::HALF) * ROLL::STEP;
}

void strike(GUI::Handle page, Whole count) {
  for (Whole line = count; line < ::drawn.size(); ++line)
    BOARDS::drop(page, STAFF::rule(line));
}

void seat(GUI::Handle page, Whole count) {
  for (Whole line = ::drawn.size(); line < count; ++line) {
    const String id = STAFF::rule(line);
    BOARDS::place(page, ROLL::board().c_str(), "panel", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
  }
}

auto seats() -> Vector<Float> {
  const Float deep = WEIGHTS::drawn(WEIGHTS::Down{WEIGHTS::LINE});
  Vector<Float> found;
  for (const Integer place : ::drawn)
    found.push_back(WEIGHTS::seated(::centre(place) - deep * ROLL::HALF));
  found.push_back(deep);
  return found;
}

void cut(GUI::Handle page) {
  const Vector<Float> seats = ::seats();
  if (seats == ::kept) return;
  ::kept = seats;
  for (Whole line = 0; line < ::drawn.size(); ++line) {
    const String id = STAFF::rule(line);
    GUI::set(page, id.c_str(), GUI::Position{::ORIGIN, seats[line]});
    GUI::set(page, id.c_str(), GUI::Extent{::reach, seats.back()});
  }
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::ruled(const Vector<Integer> &rules) {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::drawn.clear();
    ::reach = ::ORIGIN;
  }
  const Float grew = rules.empty()
                       ? ::ORIGIN
                       : GUI::SAC::ROWS::reach(
                           ::reach, GUI::NGA::GET::pan(page, board().c_str()).x,
                           window().w, ::SPARE);
  if (rules != ::drawn || grew > ::reach) {
    ::strike(page, rules.size());
    ::seat(page, rules.size());
    ::drawn = rules;
    ::reach = grew;
    ::kept.clear();
  }
  ::cut(page);
}

auto SOUND::VIEWS::ROLL::STAFF::rule(Whole line) -> String {
  return std::format("{}.{}.{}", board(), ::STEM, line);
}

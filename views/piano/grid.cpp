// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>

#include "../../boards.hpp"
#include "../../transport.hpp"
#include "piano.grid.hpp"
#include "piano.paper.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "grid";
constexpr STRING::Hot BARRED = "bar";
constexpr STRING::Hot BEATEN = "beat";
constexpr Float HAIR = 2.0f;
constexpr Float CROWD = 20.0f;
constexpr Whole COARSER = 2;

VIEWS::Pool pool;

auto dress(Flag opens) -> String {
  return String(::STEM) + (opens ? ::BARRED : ::BEATEN);
}

auto place(Integer at) -> Float {
  return at < 0 ? Float(at) / Float(VIEWS::ROLL::GRAIN)
                : VIEWS::ROLL::across(Whole(at));
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, VIEWS::ROLL::GRID::named(at));
  for (Whole at = ::pool.stood; at < count; ++at)
    BOARDS::place(
      page, VIEWS::ROLL::board().c_str(), "panel", VIEWS::ROLL::GRID::named(at),
      {}, {::HAIR, VIEWS::ROLL::STAFF::spanned().h});
  VIEWS::built(::pool, count);
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::grid() {
  VIEWS::reborn(::pool);
  ::build(0);
}

void SOUND::VIEWS::ROLL::grid() {
  const GUI::Handle page = document();
  const Vector<GUI::SAC::LADDER::Mark> marks = GRID::dressed();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, marks.size(), window().w);
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, GRID::named, marks.size());
  const Float scale = scaled();
  const GUI::Extent extent = STAFF::spanned();
  const Flag laid = papered();
  for (Whole at = 0; at < marks.size(); ++at) {
    const String id = GRID::named(at);
    const GUI::SAC::PLAYHEAD::Mark drawn =
      GUI::SAC::PLAYHEAD::blade(::place(marks[at].at), scale, ::HAIR);
    GUI::set(page, id.c_str(), GUI::Position{drawn.at, extent.w});
    if (laid && marks[at].opens) continue;
    GUI::set(page, id.c_str(), GUI::Extent{drawn.wide, extent.h});
    GUI::set(page, id.c_str(), GUI::Style{::dress(marks[at].opens).c_str()});
  }
  PAPER::grid(marks);
}

auto SOUND::VIEWS::ROLL::GRID::span() -> Whole {
  return GUI::SAC::LADDER::climb(
    PULSES, ::COARSER, VIEWS::ROLL::scaled() / Float(VIEWS::ROLL::GRAIN),
    ::CROWD, VIEWS::ROLL::window().w * Float(VIEWS::ROLL::GRAIN));
}

auto SOUND::VIEWS::ROLL::GRID::dressed() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float grain = Float(VIEWS::ROLL::GRAIN);
  const Float west =
    GUI::NGA::GET::pan(VIEWS::document(), VIEWS::ROLL::board().c_str()).x;
  return VIEWS::dressed(
    VIEWS::ROLL::sections(), {PULSES}, ::COARSER, VIEWS::ROLL::scaled() / grain,
    ::CROWD, west * grain, VIEWS::ROLL::window().w * grain);
}

auto SOUND::VIEWS::ROLL::GRID::lit() -> Whole { return ::pool.lit; }

auto SOUND::VIEWS::ROLL::GRID::hair() -> Float {
  return ::pool.stood == 0
           ? 0.0f
           : GUI::GET::measured(VIEWS::document(), named(0).c_str()).w;
}

auto SOUND::VIEWS::ROLL::GRID::named(Whole at) -> String {
  return std::format("{}.{}.{}", board(), ::STEM, at);
}

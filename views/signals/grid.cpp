// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "grid";
constexpr STRING::Hot BARRED = "bar";
constexpr STRING::Hot BEATEN = "beat";
constexpr STRING::Hot PANEL = "panel";
constexpr Float CROWD = 20.0f;

VIEWS::Pool pool;

auto mark(Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::SIGNALS::board(), ::STEM, at);
}

auto span() -> Whole {
  return GUI::SAC::LADDER::climb(
    VIEWS::SIGNALS::beat(), VIEWS::SIGNALS::COARSER,
    VIEWS::SIGNALS::scaled() / Float(VIEWS::SIGNALS::GRAIN), ::CROWD,
    VIEWS::SIGNALS::window() * Float(VIEWS::SIGNALS::GRAIN));
}

auto dressed() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float grain = Float(VIEWS::SIGNALS::GRAIN);
  const Float west =
    GUI::NGA::GET::pan(VIEWS::document(), VIEWS::SIGNALS::board().c_str()).x;
  return VIEWS::dressed(
    VIEWS::SIGNALS::sections(), {VIEWS::SIGNALS::beat()},
    VIEWS::SIGNALS::COARSER, VIEWS::SIGNALS::scaled() / grain, ::CROWD,
    west * grain, VIEWS::SIGNALS::window() * grain);
}

auto dress(Flag opens) -> String {
  return String(::STEM) + (opens ? ::BARRED : ::BEATEN);
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, ::mark(at));
  for (Whole at = ::pool.stood; at < count; ++at)
    BOARDS::place(
      page, VIEWS::SIGNALS::board().c_str(), ::PANEL, ::mark(at), {},
      {VIEWS::SIGNALS::HAIR, VIEWS::SIGNALS::DEEP});
  VIEWS::built(::pool, count);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::beat() -> Whole {
  return SCORE::framed(PULSES, TRANSPORT::held().tempos);
}

auto SOUND::VIEWS::SIGNALS::sections() -> Vector<TRANSPORT::Section> {
  return TRANSPORT::framed(TRANSPORT::sections());
}

auto SOUND::VIEWS::SIGNALS::opening() -> TRANSPORT::Section {
  const Float pan =
    GUI::NGA::GET::pan(document(), board().c_str()).x * Float(GRAIN);
  return VIEWS::standing(sections(), pan <= 0.0f ? 0 : Whole(pan));
}

auto SOUND::VIEWS::SIGNALS::bar() -> Whole { return opening().bar; }

void SOUND::VIEWS::SIGNALS::SHED::grid() {
  VIEWS::reborn(::pool);
  ::build(0);
}

void SOUND::VIEWS::SIGNALS::grid() {
  const GUI::Handle page = document();
  const Vector<GUI::SAC::LADDER::Mark> marks = ::dressed();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, marks.size(), window());
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::mark, marks.size());
  for (Whole at = 0; at < marks.size(); ++at) {
    const String id = ::mark(at);
    GUI::set(
      page, id.c_str(), GUI::Position{across(Whole(marks[at].at)), NORTH});
    GUI::set(page, id.c_str(), GUI::Style{::dress(marks[at].opens).c_str()});
  }
}

void SOUND::VIEWS::SIGNALS::grid(SHELL::Session &session) {
  const Whole step = ::span();
  const Vector<GUI::SAC::LADDER::Mark> marks = ::dressed();
  const TRANSPORT::Section one = opening();
  session.print(std::format(
    "grid marks {} beat {} step {} bar {} first {} metre {}/{}", ::pool.lit,
    beat(), step, one.numerator,
    marks.empty() || step == 0 ? 0 : Whole(marks.front().at) / step,
    one.numerator, one.denominator));
}

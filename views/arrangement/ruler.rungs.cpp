// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../score.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole SECONDS = 60;
constexpr Whole TENTH = 10;
constexpr Whole COARSER = 2;

constexpr STRING::Hot BEATS = "beat";
constexpr STRING::Hot BARS = "bar";
constexpr STRING::Hot MINUTE = "minute";
constexpr STRING::Hot TEN = "ten";
constexpr STRING::Hot HOUR = "hour";

auto barred(Whole frames) -> String {
  return VIEWS::numbered(VIEWS::ARRANGEMENT::sections(), Integer(frames));
}

auto clocked(Whole frames) -> String {
  const Whole minutes = frames / (::SECONDS * RATE);
  return std::format("{}:{:02}", minutes / ::SECONDS, minutes % ::SECONDS);
}

auto ladder(Whole bar) -> Vector<GUI::SAC::LADDER::Rung> {
  Vector<GUI::SAC::LADDER::Rung> rungs;
  const Whole minute = ::SECONDS * RATE;
  for (Whole span = VIEWS::ARRANGEMENT::beat(); span > 0 && span < bar;
       span *= ::COARSER)
    rungs.push_back({span, ::BEATS, ::barred});
  for (Whole span = bar; span > 0 && span < minute; span *= ::COARSER)
    rungs.push_back({span, ::BARS, ::barred});
  rungs.push_back({minute, ::MINUTE, ::clocked});
  rungs.push_back({::TENTH * minute, ::TEN, ::clocked});
  rungs.push_back({::SECONDS * minute, ::HOUR, ::clocked});
  return rungs;
}

auto west() -> Float {
  return GUI::NGA::GET::pan(
           VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str())
    .x;
}

auto scale() -> Float {
  return GUI::NGA::GET::zoom(
           VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str())
    .value;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::beat() -> Whole {
  return SCORE::framed(PULSES, TRANSPORT::held().tempos);
}

auto SOUND::VIEWS::ARRANGEMENT::sections() -> Vector<TRANSPORT::Section> {
  return TRANSPORT::framed(TRANSPORT::sections());
}

auto SOUND::VIEWS::ARRANGEMENT::opening() -> TRANSPORT::Section {
  const Float pan = ::west() * Float(GRAIN);
  return VIEWS::standing(sections(), pan <= 0.0f ? 0 : Whole(pan));
}

auto SOUND::VIEWS::ARRANGEMENT::bar() -> Whole { return opening().bar; }

auto SOUND::VIEWS::ARRANGEMENT::rung() -> GUI::SAC::LADDER::Rung {
  return GUI::SAC::LADDER::climb(
    ::ladder(opening().bar), ::scale() / Float(GRAIN), CROWD);
}

auto SOUND::VIEWS::ARRANGEMENT::marks() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float grain = Float(GRAIN);
  return VIEWS::dressed(
    sections(), ::ladder, ::scale() / grain, CROWD, ::west() * grain,
    window() * grain);
}

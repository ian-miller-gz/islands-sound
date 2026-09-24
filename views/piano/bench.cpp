// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot SNAP = "piano.bench.snap";
constexpr STRING::Hot LENGTH = "piano.bench.length";
constexpr STRING::Hot LOUD = "piano.bench.loud";

constexpr Whole WHOLE = 4;
constexpr Whole RUNGS[] = {32, 16, 8, 4, 2, 1};
constexpr Whole CLIMBED = sizeof(::RUNGS) / sizeof(::RUNGS[0]) - 1;
constexpr Float FINEST = 0.0f;
constexpr Float BEAT = 3.0f;

constexpr Float SHORTEST = 1.0f;
constexpr Float LONGEST = 16.0f;

constexpr Float SOFTEST = 1.0f;
constexpr Float FORTE = 100.0f;

constexpr Flag LETTERED = false;
constexpr Flag DRAWN = true;
constexpr Whole SMOOTH = 0;

Flag stood = false;
Whole born = 0;

auto rung(Whole index) -> Whole {
  return PULSES * ::WHOLE / ::RUNGS[std::min(index, ::CLIMBED)];
}

auto apart(Whole one, Whole other) -> Whole {
  return one > other ? one - other : other - one;
}

auto climbed(Whole denominator) -> Float {
  Whole nearest = 0;
  for (Whole index = 1; index <= ::CLIMBED; ++index)
    if (
      ::apart(::RUNGS[index], denominator) <
      ::apart(::RUNGS[nearest], denominator))
      nearest = index;
  return Float(nearest);
}

void read(GUI::Handle page, STRING::Hot cell, const String &text) {
  if (GUI::GET::editing(page) == cell) return;
  GUI::set(page, cell, GUI::Text{text});
}

auto typed(GUI::Handle page, STRING::Hot cell) -> Flag {
  if (!GUI::GET::committed(page, cell)) return false;
  GUI::edit(page, "");
  return true;
}

auto meant(GUI::Handle page, STRING::Hot cell) -> Float {
  return std::strtof(GUI::GET::text(page, cell).c_str(), nullptr);
}

void shaped(GUI::Handle page) {
  if (VIEWS::reborn(::born)) ::stood = false;
  GUI::set(
    page, ::SNAP,
    GUI::Dial{::FINEST, Float(::CLIMBED), ::BEAT, ::CLIMBED, ::LETTERED});
  GUI::set(
    page, ::LENGTH,
    GUI::Dial{
      ::SHORTEST, ::LONGEST, ::SHORTEST, Whole(::LONGEST - ::SHORTEST),
      ::LETTERED});
  GUI::set(
    page, ::LOUD,
    GUI::Dial{::SOFTEST, VIEWS::ROLL::FULL, ::FORTE, ::SMOOTH, ::DRAWN});
  if (::stood) return;
  GUI::set(page, ::SNAP, GUI::Value{::BEAT});
  GUI::set(page, ::LENGTH, GUI::Value{::SHORTEST});
  GUI::set(page, ::LOUD, GUI::Value{::FORTE});
  ::stood = true;
}

void taken(GUI::Handle page) {
  if (::typed(page, ::SNAP))
    GUI::set(page, ::SNAP, GUI::Value{::climbed(Whole(::meant(page, ::SNAP)))});
  if (::typed(page, ::LENGTH))
    GUI::set(page, ::LENGTH, GUI::Value{::meant(page, ::LENGTH)});
  if (::typed(page, ::LOUD))
    GUI::set(page, ::LOUD, GUI::Value{::meant(page, ::LOUD)});
}

}  // namespace

void SOUND::VIEWS::ROLL::dials() {
  const GUI::Handle page = document();
  ::shaped(page);
  ::taken(page);
  const Bench held = bench();
  ::read(page, ::SNAP, std::format("1/{}", PULSES * ::WHOLE / held.snap));
  ::read(page, ::LENGTH, std::format("{}", held.length));
  ::read(page, ::LOUD, std::format("{:.0f}", held.loud));
}

auto SOUND::VIEWS::ROLL::bench() -> Bench {
  if (!::stood) return {::rung(Whole(::BEAT)), Whole(::SHORTEST), ::FORTE};
  const GUI::Handle page = document();
  return {
    .snap = ::rung(Whole(GUI::GET::value(page, ::SNAP))),
    .length =
      std::max(Whole(::SHORTEST), Whole(GUI::GET::value(page, ::LENGTH))),
    .loud = GUI::GET::value(page, ::LOUD)};
}

void SOUND::VIEWS::ROLL::dials(SHELL::Session &session) {
  const Bench held = bench();
  session.print(std::format(
    "bench snap {} length {} loud {:g}", held.snap, held.length, held.loud));
}

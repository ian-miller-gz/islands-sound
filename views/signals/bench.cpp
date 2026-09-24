// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot SNAP = "signals.bench.snap";
constexpr Whole WHOLE = 4;
constexpr Whole RUNGS[] = {32, 16, 8, 4, 2, 1};
constexpr Whole CLIMBED = sizeof(::RUNGS) / sizeof(::RUNGS[0]) - 1;
constexpr Float FINEST = 0.0f;
constexpr Float BEAT = 3.0f;
constexpr Whole LEAST = 1;
constexpr Flag LETTERED = false;

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

auto pulsed() -> Whole {
  if (!::stood) return ::rung(Whole(::BEAT));
  return ::rung(Whole(GUI::GET::value(VIEWS::document(), ::SNAP)));
}

void shaped(GUI::Handle page) {
  if (VIEWS::reborn(::born)) ::stood = false;
  GUI::set(
    page, ::SNAP,
    GUI::Dial{::FINEST, Float(::CLIMBED), ::BEAT, ::CLIMBED, ::LETTERED});
  if (::stood) return;
  GUI::set(page, ::SNAP, GUI::Value{::BEAT});
  ::stood = true;
}

void taken(GUI::Handle page) {
  if (!GUI::GET::committed(page, ::SNAP)) return;
  GUI::edit(page, "");
  GUI::set(
    page, ::SNAP,
    GUI::Value{::climbed(
      Whole(std::strtof(GUI::GET::text(page, ::SNAP).c_str(), nullptr)))});
}

}  // namespace

void SOUND::VIEWS::SIGNALS::bench() {
  const GUI::Handle page = document();
  ::shaped(page);
  ::taken(page);
  if (GUI::GET::editing(page) == ::SNAP) return;
  GUI::set(
    page, ::SNAP,
    GUI::Text{std::format("1/{}", PULSES * ::WHOLE / ::pulsed())});
}

auto SOUND::VIEWS::SIGNALS::grain() -> Whole {
  const Whole frames = SCORE::framed(::pulsed(), TRANSPORT::held().tempos);
  return frames > ::LEAST ? frames : ::LEAST;
}

void SOUND::VIEWS::SIGNALS::bench(SHELL::Session &session) {
  session.print(std::format(
    "bench rung 1/{} grain {}", PULSES * ::WHOLE / ::pulsed(), grain()));
}

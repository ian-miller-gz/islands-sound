// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.seen.hpp"

namespace {
using namespace SOUND;

constexpr Float STILL = 0.0f, UNTURNED = 0.0f, SAME = 1.0f;
constexpr Float STEP = 1.1f;
constexpr Float NOTCH = 48.0f;

using Act = void (*)(const VIEWS::View &unit, const GUI::NGA::Wheel &turn);

struct Law {
  Flag control = false;
  Flag shift = false;
  Act act = nullptr;
};

auto centred(Float held, Float at, Float was, Float now) -> Float {
  return was == now ? held : GUI::SAC::WINDOW::about(held, at, was, now);
}

void scaled(
  const VIEWS::View &unit, const GUI::NGA::Wheel &turn, Float across,
  Float down) {
  const VIEWS::Seen held = VIEWS::GET::seen(unit);
  if (!VIEWS::zoom(unit, held.zoom.value * across, held.zoom.down * down))
    return;
  const VIEWS::Seen now = VIEWS::GET::seen(unit);
  VIEWS::pan(
    unit, ::centred(held.pan.x, turn.across, held.zoom.value, now.zoom.value),
    ::centred(held.pan.y, turn.down, held.zoom.down, now.zoom.down));
}

void scroll(const VIEWS::View &unit, const GUI::NGA::Wheel &turn) {
  const VIEWS::Seen held = VIEWS::GET::seen(unit);
  VIEWS::pan(
    unit, held.pan.x, held.pan.y - turn.turns * ::NOTCH / held.zoom.down);
}

void slide(const VIEWS::View &unit, const GUI::NGA::Wheel &turn) {
  const VIEWS::Seen held = VIEWS::GET::seen(unit);
  VIEWS::pan(
    unit, held.pan.x - turn.turns * ::NOTCH / held.zoom.value, held.pan.y);
}

void zoom(const VIEWS::View &unit, const GUI::NGA::Wheel &turn) {
  const Float by = std::pow(::STEP, turn.turns);
  ::scaled(unit, turn, by, VIEWS::GET::face(unit).split ? ::SAME : by);
}

void deepen(const VIEWS::View &unit, const GUI::NGA::Wheel &turn) {
  if (VIEWS::GET::face(unit).flat) return;
  ::scaled(unit, turn, ::SAME, std::pow(::STEP, turn.turns));
}

constexpr Law LAWS[] = {
  {false, false, scroll},
  {false, true, slide},
  {true, false, zoom},
  {true, true, deepen}};

}  // namespace

void SOUND::VIEWS::wheeled(const View &unit, const GUI::NGA::Wheel &turn) {
  if (!owns(unit.name, ZOOM) || turn.turns == ::UNTURNED) return;
  const GUI::NGA::Zoom scale = GET::seen(unit).zoom;
  if (scale.value <= ::STILL || scale.down <= ::STILL) return;
  for (const ::Law &law : ::LAWS)
    if (law.control == turn.control && law.shift == turn.shift)
      return law.act(unit, turn);
}

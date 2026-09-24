// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float CROWD = 44.0f;
constexpr Whole MINUTE = 60;

auto clocked(Whole frames) -> String {
  if (VIEWS::numbered(VIEWS::SIGNALS::sections(), Integer(frames)).empty())
    return String();
  const Whole seconds = frames / RATE;
  return std::format("{}:{:02}", seconds / ::MINUTE, seconds % ::MINUTE);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::rung() -> Whole {
  return GUI::SAC::LADDER::climb(
    beat(), COARSER, scaled() / Float(GRAIN), ::CROWD, window() * Float(GRAIN));
}

auto SOUND::VIEWS::SIGNALS::marks() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float grain = Float(GRAIN);
  const Float west = GUI::NGA::GET::pan(document(), board().c_str()).x;
  return VIEWS::dressed(
    sections(), {beat(), strip().c_str(), ::clocked}, COARSER, scaled() / grain,
    ::CROWD, west * grain, window() * grain);
}

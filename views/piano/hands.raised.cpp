// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.hands.hpp"

namespace {
using namespace SOUND;

constexpr Float WEST = 0.0f;
constexpr Float NORTH = VIEWS::ROLL::lane(VIEWS::ROLL::HIGHEST);
constexpr Float SOUTH = VIEWS::ROLL::lane(Whole(0));

}  // namespace

auto SOUND::VIEWS::ROLL::HANDS::settled(Whole pulses) -> Whole {
  const Whole snap = bench().snap;
  return (pulses + snap / 2) / snap * snap;
}

auto SOUND::VIEWS::ROLL::HANDS::shifted(
  const Pip &pip, const GUI::SAC::CARRY::Drop &landing) -> Shift {
  return {
    Integer(settled(across(landing.at.x))) - Integer(pip.at),
    Integer(lane(landing.at.y)) - Integer(pip.pitch)};
}

auto SOUND::VIEWS::ROLL::HANDS::moved(Whole at, Integer by) -> Whole {
  const Integer wanted = Integer(at) + by;
  return wanted <= 0 ? 0 : Whole(wanted);
}

auto SOUND::VIEWS::ROLL::HANDS::raised(Whole pitch, Integer by, Whole at)
  -> Whole {
  const Integer wanted = Integer(pitch) + by;
  if (wanted <= 0) return degreed(0, at);
  return degreed(wanted < Integer(HIGHEST) ? Whole(wanted) : HIGHEST, at);
}

auto SOUND::VIEWS::ROLL::watched() -> GUI::SAC::CARRY::Watch {
  return {
    board().c_str(),
    {::WEST, GUI::Walls::NONE, ::NORTH, ::SOUTH},
    HELD,
    SEATED};
}

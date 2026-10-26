// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.hands.hpp"
#include "piano.notation.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr Float WEST = 0.0f;
constexpr Float NORTH = 0.0f;
constexpr Float LIFTS[] = {0.0f, ROLL::LIFT};

auto row(Float y) -> Integer {
  return y <= ::NORTH ? 0 : Integer(y / ROLL::STEP + ROLL::HALF);
}

auto laddered(const ROLL::Pip &pip, Integer by, Whole at) -> Whole {
  const Float y = ROLL::ROWS::lane(pip.pitch, pip.at) + Float(by) * ROLL::STEP;
  const Whole pitch = ROLL::ROWS::pitch(y, at);
  return pitch == NONE ? pip.pitch : ROLL::degreed(pitch, at);
}

auto staffed(const ROLL::Pip &pip, Integer by, Whole) -> Whole {
  return ROLL::STAFF::stepped(pip.pitch, by);
}

using Step = auto (*)(const ROLL::Pip &, Integer, Whole) -> Whole;
constexpr Step STEPS[] = {::laddered, ::staffed};

}  // namespace

auto SOUND::VIEWS::ROLL::HANDS::settled(Whole pulses) -> Whole {
  const Whole snap = bench().snap;
  return (pulses + snap / 2) / snap * snap;
}

auto SOUND::VIEWS::ROLL::HANDS::shifted(
  const Pip &pip, const GUI::SAC::CARRY::Drop &landing) -> Shift {
  const Float seat = landing.at.y - ::LIFTS[GET::notation()];
  return {
    Integer(settled(across(landing.at.x))) - Integer(pip.at),
    ::row(seat) - ::row(ROWS::lane(pip.pitch, pip.at))};
}

auto SOUND::VIEWS::ROLL::HANDS::moved(Whole at, Integer by) -> Whole {
  const Integer wanted = Integer(at) + by;
  return wanted <= 0 ? 0 : Whole(wanted);
}

auto SOUND::VIEWS::ROLL::HANDS::raised(const Pip &pip, Integer by, Whole at)
  -> Whole {
  return ::STEPS[GET::notation()](pip, by, at);
}

auto SOUND::VIEWS::ROLL::watched() -> GUI::SAC::CARRY::Watch {
  return {
    board().c_str(),
    {::WEST, GUI::Walls::NONE, ::NORTH, ROWS::deep() - STEP},
    HELD,
    SEATED};
}

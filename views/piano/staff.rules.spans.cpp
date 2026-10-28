// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Float ORIGIN = 0.0f;

auto centre(Integer place) -> Float {
  return (Float(place - STAFF::top()) + ROLL::HALF) * ROLL::STEP;
}

auto tonal() -> GUI::Extent {
  if (!ROLL::GET::pitched()) return {::ORIGIN, ROLL::ROWS::deep()};
  const Float top = ::centre(STAFF::LINES[0]);
  return {top, ::centre(std::rbegin(STAFF::LINES)[0]) - top};
}

auto percussion() -> GUI::Extent {
  const Vector<Integer> rules = STAFF::GET::rules();
  if (rules.empty()) return {};
  const Float top = ::centre(rules.front());
  return {top, ::centre(rules.back()) - top};
}

constexpr auto (*SPANS[])() -> GUI::Extent = {::tonal, ::percussion};

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::spanned() -> GUI::Extent {
  return ::SPANS[ROLL::GET::notation()]();
}

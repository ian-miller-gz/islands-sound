// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <functional>
#include <utility>

#include "../../score.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Float ORIGIN = 0.0f;

struct Ladders {
  Vector<Vector<Whole>> shown;
  Vector<Whole> held;
  Vector<Key> worn;
  Whole notation = NONE, longest = ROWS::every().size(), built = 0, born = 0;
  Flag folded = false;
};

::Ladders kept;

auto lane(Whole pitch, Whole at) -> Float {
  if (!::kept.folded) return ROLL::lane(pitch);
  const Vector<Whole> &ladder = ::kept.shown[ROWS::section(::kept.worn, at)];
  const auto found = std::ranges::lower_bound(ladder, pitch, std::greater{});
  if (found == ladder.end() || *found != pitch) return ROWS::OFF;
  return Float(found - ladder.begin()) * ROLL::STEP;
}

auto pitch(Float y, Whole at) -> Whole {
  if (!::kept.folded) return ROLL::lane(y);
  const Vector<Whole> &ladder = ::kept.shown[ROWS::section(::kept.worn, at)];
  const Whole row = y <= ::ORIGIN ? 0 : Whole(y / ROLL::STEP + ROLL::HALF);
  return row < ladder.size() ? ladder[row] : ladder.back();
}

auto deep() -> Float { return Float(::kept.longest) * ROLL::STEP; }

constexpr ROWS::Ladder LADDERS[] = {
  {::lane, ::pitch, ::deep}, {STAFF::lane, STAFF::pitch, STAFF::deep}};

void build() {
  ::kept.shown.clear();
  const Whole sections = ::kept.folded ? ROLL::keyed({}).size() : 0;
  ::kept.longest = sections == 0 ? ROWS::every().size() : 0;
  for (Whole section = 0; section < sections; ++section) {
    ::kept.shown.push_back(ROWS::ladder(::kept.worn, section, ::kept.held));
    ::kept.longest = std::max(::kept.longest, ::kept.shown.back().size());
  }
  ++::kept.built;
}

}  // namespace

auto SOUND::VIEWS::ROLL::ROWS::lane(Whole pitch, Whole at) -> Float {
  return ::LADDERS[GET::notation()].lane(pitch, at);
}

auto SOUND::VIEWS::ROLL::ROWS::pitch(Float y, Whole at) -> Whole {
  return ::LADDERS[GET::notation()].pitch(y, at);
}

auto SOUND::VIEWS::ROLL::ROWS::deep() -> Float {
  return ::LADDERS[GET::notation()].deep();
}

auto SOUND::VIEWS::ROLL::ROWS::folded() -> Flag { return ::kept.folded; }

auto SOUND::VIEWS::ROLL::ROWS::shown(Whole section) -> const Vector<Whole> & {
  return section < ::kept.shown.size() ? ::kept.shown[section] : every();
}

auto SOUND::VIEWS::ROLL::ROWS::moved(Whole &seen) -> Flag {
  return std::exchange(seen, ::kept.built) != ::kept.built;
}

void SOUND::VIEWS::ROLL::ROWS::rows() {
  const Anchor held = anchored();
  const Flag reborn = VIEWS::reborn(::kept.born);
  const Flag keyed = rekeyed(::kept.worn);
  const Flag lined = STAFF::relined();
  const Flag folding = GET::notation() == TONAL && standing(::kept.worn);
  Vector<Whole> rows = ROWS::held(::kept.worn, folding);
  if (
    !reborn && !keyed && !lined && GET::notation() == ::kept.notation &&
    rows == ::kept.held)
    return;
  ::kept.held = std::move(rows);
  ::kept.notation = GET::notation();
  ::kept.folded = folding;
  ::build();
  if (!reborn) anchor(held);
}

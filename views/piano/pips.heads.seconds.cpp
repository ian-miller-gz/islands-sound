// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <ranges>
#include <tuple>

#include <island/gui/nga.hpp>

#include "piano.heads.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

struct Flip {
  Float ink = 0.0f;
  WEIGHTS::Across stem;
};
constexpr Flip FLIP = {ROLL::INK.wide * ROLL::EM, {WEIGHTS::STEM}};
constexpr Float FULL = ROLL::SPACE * ROLL::STAFF::SCALE;
constexpr Float STILL = 0.0f, EAST = 1.0f, WAYS[] = {::EAST, -::EAST};
constexpr Integer SECOND = 1;

struct Head {
  Flag foot = false;
  Whole at = 0;
  Integer place = 0;
  Whole row = 0;
};

auto footed(Whole pitch) -> Flag {
  const ROLL::Drum *drum = ROLL::GET::drum(pitch);
  return drum != nullptr && drum->foot;
}

auto heads(const Vector<ROLL::Pip> &pips) -> Vector<Head> {
  Vector<Head> found;
  for (Whole row = 0; row < pips.size(); ++row)
    if (const Integer place = STAFF::placed(pips[row].pitch);
        place != STAFF::NOWHERE)
      found.push_back({::footed(pips[row].pitch), pips[row].at, place, row});
  std::ranges::sort(found, {}, [](const Head &head) {
    return std::tie(head.foot, head.at, head.place);
  });
  return found;
}

void walk(auto &&group, Vector<Flag> &flips) {
  const Head *last = nullptr;
  for (const Head &head : group) {
    flips[head.row] = last != nullptr && !flips[last->row] &&
                      std::abs(head.place - last->place) == ::SECOND;
    last = &head;
  }
}

}  // namespace

auto SOUND::VIEWS::ROLL::HEADS::flipped(const Vector<Pip> &pips)
  -> Vector<Flag> {
  Vector<Flag> flips(pips.size(), false);
  const Vector<Head> found = ::heads(pips);
  for (const STAFF::Stem &stem : STAFF::GET::stems()) {
    const std::ranges::subrange group = std::ranges::equal_range(
      found, std::tie(stem.foot, stem.at), {},
      [](const Head &head) { return std::tie(head.foot, head.at); });
    if (stem.foot) ::walk(group, flips);
    if (!stem.foot) ::walk(group | std::views::reverse, flips);
  }
  return flips;
}

auto SOUND::VIEWS::ROLL::HEADS::shift(const Worn &now) -> Float {
  if (!now.flipped) return ::STILL;
  const Float zoom =
    GUI::NGA::GET::zoom(VIEWS::document(), board().c_str()).value;
  if (zoom <= ::STILL) return ::STILL;
  const Float flip =
    ::FLIP.ink * now.side / ::FULL / zoom - WEIGHTS::drawn(::FLIP.stem);
  return ::WAYS[::footed(now.pitch)] * flip;
}

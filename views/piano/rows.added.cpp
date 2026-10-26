// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../score.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace LANE = SOUND::VIEWS::LANE;

auto keyed(const LANE::Added &one, const Key &key) -> Flag {
  return one.tonic == key.tonic && one.scale == key.scale;
}

auto scaled(const Key &key, Whole pitch) -> Flag {
  return SCORE::degree(key, pitch) != NONE;
}

}  // namespace

auto SOUND::VIEWS::ROLL::ROWS::added(const Key &key) -> Vector<Whole> {
  Vector<Whole> pitches;
  for (const LANE::Added &one : faced().added)
    if (::keyed(one, key)) pitches.push_back(one.pitch);
  std::ranges::sort(pitches);
  return pitches;
}

void SOUND::VIEWS::ROLL::ROWS::add(const Key &key, Whole pitch) {
  Vector<LANE::Added> &added = faced().added;
  const auto found = std::ranges::find_if(added, [&](const LANE::Added &one) {
    return ::keyed(one, key) && one.pitch == pitch;
  });
  if (found != added.end())
    added.erase(found);
  else
    added.push_back({.tonic = key.tonic, .scale = key.scale, .pitch = pitch});
}

auto SOUND::VIEWS::ROLL::ROWS::gaps(const Key &key) -> Vector<Gap> {
  Vector<Gap> found;
  if (!::scaled(key, key.tonic)) return found;
  Flag above = false;
  for (Whole pitch : every())
    if (::scaled(key, pitch))
      above = true;
    else if (above && !found.empty() && found.back().low == pitch + 1)
      found.back().low = pitch;
    else if (above)
      found.push_back({pitch, pitch});
  return found;
}

auto SOUND::VIEWS::ROLL::ROWS::named(Whole pitch) -> String {
  return spelled(pitch);
}

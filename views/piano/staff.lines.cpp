// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../transport.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

Vector<STAFF::Line> kept;
Key keyed;
Vector<STAFF::Line> spelt;
Vector<Whole> signs;

auto pitches() -> Vector<Whole> {
  Vector<Whole> found;
  for (const ROLL::Pip &pip : ROLL::pips()) found.push_back(pip.pitch);
  std::ranges::sort(found);
  found.erase(std::ranges::unique(found).begin(), found.end());
  return found;
}

auto composed() -> Vector<STAFF::Line> {
  if (ROLL::GET::pitched()) return STAFF::pitched(TRANSPORT::keyed(0));
  if (ROLL::GET::notation() != ROLL::PERCUSSION) return {};
  const Vector<STAFF::Line> lines =
    STAFF::placed(ROLL::GET::voices(), ::pitches());
  if (ROLL::GET::presentation() != ROLL::PRESENTATION::GRID) return lines;
  return STAFF::gridded(lines);
}

auto same(const STAFF::Line &one, const STAFF::Line &two) -> Flag {
  return one.place == two.place && one.note == two.note && one.name == two.name;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::GET::lines() -> const Vector<Line> & {
  return ::kept;
}

auto SOUND::VIEWS::ROLL::STAFF::placed(Whole pitch) -> Integer {
  for (const Line &line : ::kept)
    if (line.note == pitch) return line.place;
  return NOWHERE;
}

auto SOUND::VIEWS::ROLL::STAFF::relined() -> Flag {
  Vector<Line> lines = ::composed();
  if (std::ranges::equal(lines, ::kept, ::same)) return false;
  ::kept = std::move(lines);
  return true;
}

auto SOUND::VIEWS::ROLL::STAFF::pitched(const Key &key) -> Vector<Line> {
  const Flag same = key.tonic == ::keyed.tonic && key.scale == ::keyed.scale;
  if (same && !::spelt.empty()) return ::spelt;
  ::keyed = key;
  ::spelt.clear();
  ::signs.assign(HIGHEST + 1, NONE);
  for (Whole pitch = 0; pitch <= HIGHEST; ++pitch) {
    const Spelt note = spelled(key, pitch);
    ::spelt.push_back({note.place, pitch, note.name});
    ::signs[pitch] = note.sign;
  }
  std::ranges::stable_sort(::spelt, [](const Line &one, const Line &two) {
    if (one.place != two.place) return one.place < two.place;
    return (::signs[one.note] == NONE) > (::signs[two.note] == NONE);
  });
  return ::spelt;
}

auto SOUND::VIEWS::ROLL::STAFF::accidental(Whole pitch) -> Whole {
  return pitch < ::signs.size() ? ::signs[pitch] : NONE;
}

auto SOUND::VIEWS::ROLL::STAFF::top() -> Integer {
  if (!ROLL::GET::pitched() || ::kept.empty()) return TOP;
  const Integer highest = ::kept.front().place;
  return highest - (highest % 2 + 2) % 2;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../score.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;

auto standing(const Key &key) -> Flag {
  return SCORE::degree(key, key.tonic) != NONE;
}

auto keyed(const Vector<Key> &keys, Whole section) -> Key {
  return section < keys.size() ? keys[section] : Key{};
}

auto code(Whole section, Whole pitch) -> Whole {
  return section * ROWS::every().size() + pitch;
}

auto off(const Key &key, Whole pitch) -> Flag {
  return ::standing(key) && SCORE::degree(key, pitch) == NONE;
}

auto kept(const Key &key, Whole section, Whole pitch, const Vector<Whole> &held)
  -> Flag {
  if (!::off(key, pitch)) return true;
  return std::ranges::binary_search(held, ::code(section, pitch));
}

}  // namespace

auto SOUND::VIEWS::ROLL::ROWS::every() -> const Vector<Whole> & {
  static Vector<Whole> pitches;
  if (pitches.empty())
    for (Whole row = 0; row <= HIGHEST; ++row) pitches.push_back(HIGHEST - row);
  return pitches;
}

auto SOUND::VIEWS::ROLL::ROWS::standing(const Vector<Key> &keys) -> Flag {
  return std::ranges::any_of(keys, ::standing);
}

auto SOUND::VIEWS::ROLL::ROWS::section(const Vector<Key> &keys, Whole at)
  -> Whole {
  Whole found = 0;
  for (Whole row = 0; row < keys.size(); ++row)
    if (keys[row].at <= at) found = row;
  return found;
}

auto SOUND::VIEWS::ROLL::ROWS::held(const Vector<Key> &keys, Flag folding)
  -> Vector<Whole> {
  Vector<Whole> held;
  if (!folding) return held;
  Vector<Pip> plates = pips();
  loosed(plates);
  for (const Pip &plate : plates) {
    const Whole section = ROWS::section(keys, plate.at);
    if (::off(::keyed(keys, section), plate.pitch))
      held.push_back(::code(section, plate.pitch));
  }
  for (Whole section = 0; section < keys.size(); ++section)
    for (Whole pitch : ROWS::added(keys[section]))
      if (::off(keys[section], pitch)) held.push_back(::code(section, pitch));
  std::ranges::sort(held);
  held.erase(std::ranges::unique(held).begin(), held.end());
  return held;
}

auto SOUND::VIEWS::ROLL::ROWS::ladder(
  const Vector<Key> &keys, Whole section,
  const Vector<Whole> &held) -> Vector<Whole> {
  const Key key = ::keyed(keys, section);
  Vector<Whole> pitches;
  for (Whole pitch : every())
    if (::kept(key, section, pitch, held)) pitches.push_back(pitch);
  return pitches;
}

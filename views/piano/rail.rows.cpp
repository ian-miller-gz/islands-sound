// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../score.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

struct Share {
  Float west = 0.0f, wide = ROLL::RAILED;
};

auto shared(Whole pitch) -> Share {
  const Integer place = STAFF::placed(pitch);
  Whole slot = 0, count = 0;
  for (const STAFF::Line &line : STAFF::GET::lines()) {
    if (line.place != place) continue;
    if (line.note == pitch) slot = count;
    ++count;
  }
  if (count == 0) return {};
  const Float wide = ROLL::RAILED / Float(count);
  return {Float(slot) * wide, wide};
}

}  // namespace

auto SOUND::VIEWS::ROLL::ROWS::lettered(Whole pitch, const Key &key) -> String {
  for (const STAFF::Line &line : STAFF::GET::lines())
    if (line.note == pitch) return line.name;
  const String name = ROLL::named(pitch);
  const Whole degree = SCORE::degree(key, pitch);
  if (degree == NONE) return name;
  return name.empty() ? std::format("{}", degree)
                      : std::format("{} {}", degree, name);
}

auto SOUND::VIEWS::ROLL::GET::railed() -> Vector<Whole> {
  if (GET::notation() == TONAL) return ROWS::every();
  Vector<Whole> notes;
  for (const STAFF::Line &line : STAFF::GET::lines())
    notes.push_back(line.note);
  return notes;
}

void SOUND::VIEWS::ROLL::ROWS::railed(
  GUI::Handle page, STRING::Hot stem, Whole section, Whole opens, Float x,
  Flag shown) {
  for (Whole pitch = 0; pitch <= HIGHEST; ++pitch) {
    const String id = sectioned(stem, section, pitch);
    const Float y = lane(pitch, opens);
    const ::Share share = ::shared(pitch);
    GUI::set(page, id.c_str(), GUI::Visibility{shown && y != OFF});
    GUI::set(page, id.c_str(), GUI::Position{x + share.west, y});
    GUI::set(page, id.c_str(), GUI::Extent{share.wide, STEP});
    GUI::set(page, id.c_str(), GUI::Clipping{true});
  }
}

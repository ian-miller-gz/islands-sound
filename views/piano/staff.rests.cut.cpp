// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "piano.beams.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Whole SHORTEST = PULSES / std::rbegin(STAFF::VALUES)->part;
constexpr Flag HANDS = false;

struct Bar {
  Whole at = 0, close = 0;
};

auto barred(Whole pulse) -> Bar {
  const TRANSPORT::Section section = VIEWS::standing(ROLL::sections(), pulse);
  const Whole span = section.bar > 0 ? section.bar : PULSES;
  const Whole into = pulse > section.at ? pulse - section.at : 0;
  const Whole at = pulse - into % span;
  return {at, at + span};
}

auto lasting(const STAFF::Value &value) -> Whole { return PULSES / value.part; }

void cut(Whole beat, Whole from, Whole to, Vector<STAFF::Rest> &rests) {
  Whole at = from;
  while (at < to) {
    const auto fits = [beat, at, to](const STAFF::Value &value) {
      return (at - beat) % ::lasting(value) == 0 && at + ::lasting(value) <= to;
    };
    const auto value = std::ranges::find_if(STAFF::VALUES, fits);
    if (value == std::end(STAFF::VALUES)) {
      at = beat + ((at - beat) / ::SHORTEST + 1) * ::SHORTEST;
      continue;
    }
    rests.push_back({at, value->beams});
    at += ::lasting(*value);
  }
}

void cut(
  Whole beat, const Vector<STAFF::Stem> &voice, Vector<STAFF::Rest> &rests) {
  const Whole close = beat + PULSES;
  Whole at = beat;
  for (const STAFF::Stem &stem : voice) {
    if (stem.at < beat || stem.at >= close) continue;
    if (stem.at > at) ::cut(beat, at, stem.at, rests);
    const Whole held = stem.at + ::lasting(STAFF::VALUES[stem.beams]);
    at = std::max(at, std::min(close, held));
  }
  ::cut(beat, at, close, rests);
}

void rest(
  const Vector<STAFF::Stem> &stems, Flag foot, Vector<STAFF::Rest> &rests) {
  Vector<STAFF::Stem> voice;
  std::ranges::copy_if(
    stems, std::back_inserter(voice),
    [foot](const STAFF::Stem &stem) { return stem.foot == foot; });
  if (voice.empty()) return;
  const Whole close = ::barred(voice.back().at).close;
  for (Whole beat = ::barred(voice.front().at).at; beat < close;
       beat = STAFF::beaten(beat) + PULSES)
    ::cut(beat, voice, rests);
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::rested(const Vector<Stem> &stems)
  -> Vector<Rest> {
  Vector<Rest> rests;
  if (ROLL::GET::presentation() == ROLL::PRESENTATION::GRID) return rests;
  ::rest(stems, ::HANDS, rests);
  return rests;
}

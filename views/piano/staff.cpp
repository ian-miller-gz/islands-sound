// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../score.hpp"
#include "piano.heads.hpp"
#include "piano.rows.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
using AUDIO::PLUGIN::Voice;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

struct Staff {
  Vector<STAFF::Line> lines;
  Integer stray = STAFF::STRAYS;
};

void line(Staff &staff, Whole note, const String &name) {
  const ROLL::Drum *drum = ROLL::GET::drum(note);
  if (drum != nullptr) return staff.lines.push_back({drum->place, note, name});
  staff.lines.push_back({staff.stray++, note, name});
}

auto named(const Vector<Voice> &voices, Whole pitch) -> Flag {
  return std::ranges::any_of(
    voices, [pitch](const Voice &voice) { return voice.note == pitch; });
}

auto worded(Whole pitch) -> String {
  const ROLL::Drum *drum = ROLL::GET::drum(pitch);
  return drum != nullptr ? String(drum->name) : ROLL::spelled(pitch);
}

auto before(const STAFF::Line &one, const STAFF::Line &two) -> Flag {
  if (one.place != two.place) return one.place < two.place;
  const ROLL::Drum *west = ROLL::GET::drum(one.note);
  const ROLL::Drum *east = ROLL::GET::drum(two.note);
  if (west != east) return std::less{}(west, east);
  return one.note < two.note;
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::placed(
  const Vector<Voice> &voices, const Vector<Whole> &pitches) -> Vector<Line> {
  ::Staff staff;
  for (const Voice &voice : voices) ::line(staff, voice.note, voice.name);
  for (Whole pitch : pitches)
    if (!::named(voices, pitch)) ::line(staff, pitch, ::worded(pitch));
  std::ranges::stable_sort(staff.lines, ::before);
  return staff.lines;
}

auto SOUND::VIEWS::ROLL::spelled(Whole pitch) -> String {
  return std::format(
    "{}{}", SCORE::classed(pitch % CLASSES),
    Integer(pitch / CLASSES) - ROWS::OCTAVE);
}

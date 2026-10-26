// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "../../timeline.hpp"
#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
using TIMELINE::SHIFT::Interval;

constexpr Whole SIDES = 2;

struct Staged {
  Flag standing = false;
  Flag stirred = false;
  Interval interval;
};

Staged staged;

auto standing() -> Interval {
  if (::staged.standing) return ::staged.interval;
  const Whole row = WAVE::chosen();
  if (row == NONE) return {};
  return TIMELINE::SHIFT::split(WAVE::placed()[row].cents);
}

void stage(Integer Interval::*part, Float value) {
  Interval interval = ::standing();
  interval.*part = Integer(std::lround(value));
  ::staged = {true, true, interval};
}

namespace PITCH {

constexpr STRING::Hot NAME = "Pitch";

auto published() -> AUDIO::PLUGIN::Control {
  constexpr Float span = Float(TIMELINE::SHIFT::SPAN);
  return {
    .steps = Whole(TIMELINE::SHIFT::SPAN) * ::SIDES,
    .least = -span,
    .most = span};
}

auto read() -> Float { return Float(::standing().semitones); }

void write(Float value) { ::stage(&Interval::semitones, value); }

}  // namespace PITCH

namespace CENTS {

constexpr STRING::Hot NAME = "Cents";

auto published() -> AUDIO::PLUGIN::Control {
  constexpr Float fine = Float(TIMELINE::SHIFT::FINE);
  return {.least = -fine, .most = fine};
}

auto read() -> Float { return Float(::standing().cents); }

void write(Float value) { ::stage(&Interval::cents, value); }

}  // namespace CENTS

}  // namespace

auto SOUND::VIEWS::WAVE::SHIFT::pitch() -> RIG::Source {
  return {
    .name = ::PITCH::NAME,
    .published = ::PITCH::published,
    .read = ::PITCH::read,
    .write = ::PITCH::write};
}

auto SOUND::VIEWS::WAVE::SHIFT::cents() -> RIG::Source {
  return {
    .name = ::CENTS::NAME,
    .published = ::CENTS::published,
    .read = ::CENTS::read,
    .write = ::CENTS::write};
}

void SOUND::VIEWS::WAVE::SHIFT::commit() {
  const Flag waiting = ::staged.stirred || INPUT::GET::pointer().left;
  ::staged.stirred = false;
  if (!::staged.standing || waiting) return;
  ::staged.standing = false;
  const Whole row = chosen();
  if (row == NONE) return;
  const Clip was = placed()[row];
  const Clip now =
    TIMELINE::SHIFT::shifted(was, TIMELINE::SHIFT::joined(::staged.interval));
  if (now.cents != was.cents) retouch(row, was, now, TIMELINE::SHIFT::WORD);
}

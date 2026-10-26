// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "../views.tune.hpp"
#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace TUNE = SOUND::TIMELINE::TUNE;

constexpr STRING::Hot NAME = "Tune";

struct Staged {
  Flag standing = false;
  Flag stirred = false;
  Float strength = TUNE::STRENGTH;
};

Staged staged;
Float strength = TUNE::STRENGTH;

auto published() -> AUDIO::PLUGIN::Control {
  return {.resting = TUNE::STRENGTH, .least = TUNE::LEAST, .most = TUNE::MOST};
}

auto read() -> Float {
  if (::staged.standing) return ::staged.strength;
  const Whole row = WAVE::chosen();
  if (row == NONE || WAVE::placed()[row].tunes.empty()) return TUNE::LEAST;
  return ::strength;
}

void write(Float value) { ::staged = {true, true, value}; }

}  // namespace

auto SOUND::VIEWS::WAVE::TUNE::source() -> RIG::Source {
  return {
    .name = ::NAME, .published = ::published, .read = ::read, .write = ::write};
}

void SOUND::VIEWS::WAVE::TUNE::commit() {
  const Flag waiting = ::staged.stirred || INPUT::GET::pointer().left;
  ::staged.stirred = false;
  if (!::staged.standing || waiting) return;
  ::staged.standing = false;
  const LANE::Mark mark = laned();
  const Whole row = chosen();
  if (mark.lane == NONE || row == NONE) return;
  ::strength = ::staged.strength;
  VIEWS::tuned(mark.track, mark.lane, row, ::strength);
}

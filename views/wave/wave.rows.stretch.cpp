// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace RATIO = TIMELINE::RATIO;

constexpr STRING::Hot NAME = "Stretch";
constexpr STRING::Hot UNIT = "%";

struct Staged {
  Flag standing = false;
  Flag stirred = false;
  Float percent = 0.0f;
};

Staged staged;

auto published() -> AUDIO::PLUGIN::Control {
  return {
    .unit = ::UNIT,
    .resting = RATIO::PERCENT,
    .least = RATIO::LEAST,
    .most = RATIO::MOST};
}

auto read() -> Float {
  if (::staged.standing) return ::staged.percent;
  const Whole row = WAVE::chosen();
  if (row == NONE) return RATIO::PERCENT;
  return WAVE::placed()[row].ratio * RATIO::PERCENT;
}

void write(Float value) { ::staged = {true, true, RATIO::held(value)}; }

}  // namespace

auto SOUND::VIEWS::WAVE::STRETCH::source() -> RIG::Source {
  return {
    .name = ::NAME, .published = ::published, .read = ::read, .write = ::write};
}

void SOUND::VIEWS::WAVE::STRETCH::commit() {
  const Flag waiting = ::staged.stirred || INPUT::GET::pointer().left;
  ::staged.stirred = false;
  if (!::staged.standing || waiting) return;
  ::staged.standing = false;
  const Whole row = chosen();
  if (row == NONE) return;
  const Clip was = placed()[row];
  const Clip now = RATIO::stretched(was, ::staged.percent);
  if (now.frames != was.frames || now.ratio != was.ratio)
    retouch(row, was, now, RATIO::WORD);
}

auto SOUND::VIEWS::WAVE::worded(const Clip &clip) -> String {
  return RIG::worded(::published(), clip.ratio * RATIO::PERCENT);
}

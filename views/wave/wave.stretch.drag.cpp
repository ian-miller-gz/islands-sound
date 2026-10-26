// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../../timeline.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace RATIO = TIMELINE::RATIO;

constexpr Integer SHORTEST = 1;

struct Landing {
  Whole row = NONE;
  WAVE::Clip clip;
};

Landing landing;

auto over(const WAVE::Clip &clip, GUI::Position at) -> Flag {
  const GUI::Extent seat = WAVE::handled();
  const Float east = Float(clip.at + clip.frames);
  const Float south = WAVE::deep();
  return at.x >= east - seat.w && at.x <= east && at.y >= south - seat.h &&
         at.y <= south;
}

auto grasped(const Vector<WAVE::Clip> &seated, GUI::Position at) -> Whole {
  for (Whole row = 0; row < seated.size(); ++row)
    if (WAVE::shown(seated[row]) && ::over(seated[row], at)) return row;
  return NONE;
}

auto drawn(const WAVE::Clip &clip, Float travel) -> WAVE::Clip {
  const Integer frames =
    std::max(::SHORTEST, Integer(clip.frames) + Integer(std::lround(travel)));
  return RATIO::drawn(clip, Whole(frames));
}

}  // namespace

auto SOUND::VIEWS::WAVE::held() -> Whole { return ::landing.row; }

auto SOUND::VIEWS::WAVE::landed(Whole row, const Clip &clip) -> Clip {
  return row == ::landing.row ? ::landing.clip : clip;
}

auto SOUND::VIEWS::WAVE::stretch(const GUI::SAC::STROKE::Gesture &gesture)
  -> Flag {
  ::landing = {};
  if (!gesture.standing && !gesture.closed) return false;
  const Vector<Clip> seated = placed();
  const Whole row = ::grasped(seated, gesture.from.at);
  if (row == NONE) return false;
  if (!LANE::GET::admitted(seated[row].at)) return true;
  const Clip now = ::drawn(seated[row], gesture.to.at.x - gesture.from.at.x);
  if (!gesture.closed) {
    ::landing = {row, now};
    return true;
  }
  if (gesture.dragged && now.frames != seated[row].frames)
    retouch(row, seated[row], now, RATIO::WORD);
  return true;
}

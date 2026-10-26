// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <iterator>

#include "../../fade.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

auto over(const WAVE::Clip &clip, Whole end, GUI::Position at) -> Flag {
  const GUI::Extent seat = WAVE::gripped();
  const Float from = WAVE::west(clip, end);
  return at.x >= from && at.x <= from + seat.w && at.y >= WAVE::ORIGIN &&
         at.y <= seat.h;
}

void dragged(Whole row, const WAVE::Clip &was, const FADE::End &end, Float by) {
  const Integer travel = Integer(std::lround(end.east ? -by : by));
  const FADE::Fade &standing = was.*end.fade;
  WAVE::Clip now = was;
  (now.*end.fade).frames =
    FADE::held(Integer(standing.frames) + travel, FADE::room(was, end));
  if ((now.*end.fade).frames != standing.frames)
    WAVE::retouch(row, was, now, FADE::WORD);
}

}  // namespace

auto SOUND::VIEWS::WAVE::fade(const GUI::SAC::STROKE::Gesture &gesture)
  -> Flag {
  const Vector<Clip> seated = placed();
  const Float travel = gesture.to.at.x - gesture.from.at.x;
  for (Whole row = 0; row < seated.size(); ++row)
    for (Whole end = 0; end < std::size(FADE::ENDS); ++end)
      if (shown(seated[row]) && ::over(seated[row], end, gesture.from.at)) {
        if (LANE::GET::admitted(seated[row].at))
          ::dragged(row, seated[row], FADE::ENDS[end], travel);
        return true;
      }
  return false;
}

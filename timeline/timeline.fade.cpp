// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../fade.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Fade;

constexpr Integer BACK = -1;

struct Ramp {
  Whole from = 0, until = 0;
  Integer start = 0, step = 1;
};

void faded(Vector<Float> &lane, Whole offset, const Fade &fade, Ramp ramp) {
  const FADE::Shape &shape = FADE::shaped(fade.shape);
  const Float length = Float(fade.frames);
  const Whole until = std::min(ramp.until, offset + lane.size());
  for (Whole at = std::max(ramp.from, offset); at < until; ++at)
    lane[at - offset] *=
      shape.gain(Float(ramp.start + ramp.step * Integer(at)) / length);
}

}  // namespace

void SOUND::TIMELINE::fade(
  Laid &stretch, const ARRANGEMENT::TRACK::LANE::Clip &span, Whole offset) {
  const Whole frames = span.frames;
  const Whole out = std::min(span.out.frames, frames);
  for (Vector<Float> &lane : stretch.lanes) {
    if (span.in.frames != 0)
      ::faded(lane, offset, span.in, {.until = span.in.frames});
    if (out != 0)
      ::faded(
        lane, offset, span.out,
        {frames - out, frames, Integer(frames), ::BACK});
  }
}

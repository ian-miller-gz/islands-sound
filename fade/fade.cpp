// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "fade.hpp"

using namespace SOUND;

auto SOUND::FADE::shaped(Whole shape) -> const Shape & {
  return SHAPES[shape < std::size(SHAPES) ? shape : 0];
}

auto SOUND::FADE::room(const Clip &clip, const End &end) -> Whole {
  return clip.frames - std::min(clip.frames, (clip.*end.other).frames);
}

auto SOUND::FADE::knee(const Clip &clip, const End &end) -> Whole {
  const Whole frames = std::min(clip.frames, (clip.*end.fade).frames);
  return end.east ? clip.frames - frames : frames;
}

auto SOUND::FADE::held(Integer frames, Whole room) -> Whole {
  return std::min(Whole(std::max(frames, Integer(FLOOR))), room);
}

auto SOUND::FADE::gain(const Fade &fade, Whole along) -> Float {
  if (along >= fade.frames) return FULL;
  return shaped(fade.shape).gain(Float(along) / Float(fade.frames));
}

auto SOUND::FADE::gain(const Clip &clip, Whole offset) -> Float {
  if (offset >= clip.frames) return FULL;
  return gain(clip.in, offset) * gain(clip.out, clip.frames - offset);
}

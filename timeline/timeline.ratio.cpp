// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "timeline.ratio.hpp"

namespace RATIO = SOUND::TIMELINE::RATIO;

auto RATIO::held(Float percent) -> Float {
  return std::clamp(percent, LEAST, MOST);
}

auto RATIO::covered(const Clip &clip) -> Whole {
  if (clip.ratio == Clip{}.ratio) return clip.frames;
  return Whole(std::lround(Float(clip.frames) / clip.ratio));
}

auto RATIO::stretched(const Clip &clip, Float percent) -> Clip {
  Clip now = clip;
  const Whole source = covered(clip);
  now.ratio = held(percent) / PERCENT;
  now.frames = Whole(std::lround(Float(source) * now.ratio));
  return now;
}

auto RATIO::drawn(const Clip &clip, Whole frames) -> Clip {
  const Whole source = covered(clip);
  if (source == 0) return clip;
  const Float ratio = Float(frames) / Float(source);
  if (held(ratio * PERCENT) != ratio * PERCENT)
    return stretched(clip, ratio * PERCENT);
  Clip now = clip;
  now.ratio = ratio;
  now.frames = frames;
  return now;
}

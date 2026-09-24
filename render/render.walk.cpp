// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"

auto SOUND::RENDER::walk(Whole start, Whole frames, const Span &cycle) -> Walk {
  Walk shaped = {.start = start};
  if (start == NONE || cycle.to <= cycle.from) return shaped;
  if (start >= cycle.to || start + frames < cycle.to) return shaped;
  shaped.cut = cycle.to - start;
  shaped.from = cycle.from;
  return shaped;
}

auto SOUND::RENDER::after(const Walk &walk, Whole frames) -> Whole {
  if (walk.start == NONE) return NONE;
  return walk.cut == 0 ? walk.start + frames : walk.from + frames - walk.cut;
}

auto SOUND::RENDER::legs(const Walk &walk, Whole frames, Leg (&out)[LEGS])
  -> Whole {
  if (walk.start == NONE) return 0;
  if (walk.cut == 0) {
    out[0] = {walk.start, 0, frames};
    return 1;
  }
  out[0] = {walk.start, 0, walk.cut};
  out[1] = {walk.from, walk.cut, frames - walk.cut};
  return LEGS;
}

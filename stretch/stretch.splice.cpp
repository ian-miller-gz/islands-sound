// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "stretch.internal.hpp"

namespace {
using namespace SOUND;
using STRETCH::JOIN;
using STRETCH::Span;

auto slice(const Take &take, Whole from, Whole to) -> Take {
  Take piece = {.path = take.path, .lanes = {}};
  for (const Vector<Float> &lane : take.lanes)
    piece.lanes.push_back(
      {lane.begin() + std::min(from, lane.size()),
       lane.begin() + std::min(to, lane.size())});
  return piece;
}

auto gain(Whole at, Whole end, Whole place) -> Float {
  const Whole apart = place < at    ? at - place
                      : place < end ? 0
                                    : place - end + 1;
  return Float(JOIN - apart) / Float(JOIN);
}

void join(Take &made, const Take &piece, Whole from, Whole at, Whole end) {
  for (Whole lane = 0; lane < made.lanes.size(); ++lane) {
    Vector<Float> &target = made.lanes[lane];
    const Vector<Float> &shifted = piece.lanes[lane];
    const Whole to = std::min(from + shifted.size(), target.size());
    for (Whole place = from; place < to; ++place)
      target[place] +=
        (shifted[place - from] - target[place]) * ::gain(at, end, place);
  }
}

void splice(Take &made, const Take &take, const Span &span) {
  const Whole frames = STRETCH::length(take);
  if (span.frames == 0 || span.at >= frames) return;
  const Whole end = std::min(span.at + span.frames, frames);
  const Whole from = span.at > JOIN ? span.at - JOIN : 0;
  const Whole to = std::min(end + JOIN, frames);
  const Take piece =
    STRETCH::stretched(::slice(take, from, to), STRETCH::SAME, span.cents);
  ::join(made, piece, from, span.at, end);
}

}  // namespace

auto SOUND::STRETCH::spliced(const Take &take, const Vector<Span> &spans)
  -> Take {
  Take made = take;
  for (const Span &span : spans) ::splice(made, take, span);
  return made;
}

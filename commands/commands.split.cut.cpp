// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../kind.hpp"
#include "../score.hpp"
#include "../transport.hpp"
#include "commands.split.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;
using ARRANGEMENT::TRACK::LANE::Tune;

auto pulsed(Whole frames) -> Whole {
  return SCORE::pulsed(frames, TRANSPORT::held().tempos);
}

auto advanced(const Clip &span, Whole kind, Whole cut) -> Whole {
  if (kind == KIND::NOTES) return ::pulsed(span.at + cut) - ::pulsed(span.at);
  if (span.ratio == Clip{}.ratio) return cut;
  return Whole(std::lround(Float(cut) / span.ratio));
}

auto parted(const Vector<Tune> &tunes, Whole point, Flag later)
  -> Vector<Tune> {
  Vector<Tune> kept;
  for (const Tune &tune : tunes)
    if (later ? tune.at + tune.frames > point : tune.at < point)
      kept.push_back(tune);
  return kept;
}

}  // namespace

auto SOUND::COMMANDS::SPLIT::cut(const Clip &span, Whole kind, Whole at)
  -> Vector<Clip> {
  Clip first = span, second = span;
  first.frames = at - span.at;
  first.out = {};
  first.in.frames = std::min(first.in.frames, first.frames);
  second.at = at;
  second.frames = span.frames - first.frames;
  second.from = span.from + ::advanced(span, kind, first.frames);
  second.in = {};
  second.out.frames = std::min(second.out.frames, second.frames);
  first.tunes = ::parted(span.tunes, second.from, false);
  second.tunes = ::parted(span.tunes, second.from, true);
  return {first, second};
}

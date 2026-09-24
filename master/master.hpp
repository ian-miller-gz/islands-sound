// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../graph.hpp"

namespace SOUND::MASTER {

using Digest = Word;

struct Tally {
  Whole frames = 0;
  Whole rate = RATE;
  Whole channels = CHANNELS;
  Whole notes = 0;
  Digest digest = 0;
};

auto bounce(Whole frames, Tally &out) -> Flag;
auto bounce(Whole from, Whole frames, Tally &out) -> Flag;

auto bounce(
  const Vector<String> &nodes, Whole from, Whole frames,
  Vector<Vector<Float>> &lanes) -> Flag;

namespace WAVE {

auto write(const String &path, Whole rate, const Vector<Vector<Float>> &lanes)
  -> Flag;

}  // namespace WAVE

}  // namespace SOUND::MASTER

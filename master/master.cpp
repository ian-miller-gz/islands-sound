// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../graph.hpp"
#include "master.hpp"
#include "../render.hpp"

namespace {
using namespace SOUND;
using MASTER::Digest;

constexpr Digest SEED = 2166136261u;
constexpr Digest PRIME = 16777619u;

void fold(
  Digest &digest, const Vector<Vector<AUDIO::PLUGIN::Sample>> &lanes,
  Whole frames) {
  for (Whole frame = 0; frame < frames; ++frame)
    for (const Vector<AUDIO::PLUGIN::Sample> &lane : lanes) {
      const Float held = std::clamp(lane[frame], -1.0f, 1.0f);
      const Integer pulse = static_cast<Integer>(held * (SCALE - 1.0f));
      digest = (digest ^ Digest(pulse & 0xff)) * PRIME;
      digest = (digest ^ Digest((pulse >> 8) & 0xff)) * PRIME;
    }
}

void gathered(
  Vector<Vector<Float>> &lanes, const Vector<String> &nodes, Whole made,
  Whole span) {
  for (const String &node : nodes) {
    const Vector<Vector<AUDIO::PLUGIN::Sample>> &sounded =
      RENDER::sounded(node);
    if (lanes.size() < sounded.size()) lanes.resize(sounded.size());
    for (Whole lane = 0; lane < sounded.size(); ++lane) {
      if (lanes[lane].size() < made + span)
        lanes[lane].resize(made + span, 0.0f);
      for (Whole frame = 0; frame < span && frame < sounded[lane].size();
           ++frame)
        lanes[lane][made + frame] += sounded[lane][frame];
    }
  }
}

template <typename Each>
auto rendered(Whole from, Whole frames, Each each) -> Flag {
  const Flag stood = RENDER::VOICE::standing();
  RENDER::VOICE::drop();
  RENDER::plan();
  GRAPH::rest();
  RENDER::forget();
  Flag whole = true;
  Whole at = from;
  for (Whole made = 0; whole && made < frames; made += RENDER::BLOCK) {
    const Whole span = std::min<Whole>(RENDER::BLOCK, frames - made);
    const RENDER::Walk walk = RENDER::walk(at, span);
    whole = RENDER::block(walk, span);
    if (whole) each(made, span);
    at = RENDER::after(walk, span);
  }
  if (stood) RENDER::VOICE::claim();
  return whole;
}

}  // namespace

auto SOUND::MASTER::bounce(
  const Vector<String> &nodes, Whole from, Whole frames,
  Vector<Vector<Float>> &lanes) -> Flag {
  if (nodes.empty()) return false;
  for (const String &node : nodes)
    if (GRAPH::at(node) == NONE) return false;
  lanes.clear();
  return ::rendered(from, frames, [&](Whole made, Whole span) {
    ::gathered(lanes, nodes, made, span);
  });
}

auto SOUND::MASTER::bounce(Whole frames, Tally &out) -> Flag {
  return bounce(0, frames, out);
}

auto SOUND::MASTER::bounce(Whole from, Whole frames, Tally &out) -> Flag {
  const String clock = GRAPH::clock();
  out = {.frames = frames};
  Digest digest = ::SEED;
  const Flag whole = ::rendered(from, frames, [&](Whole, Whole span) {
    out.notes += RENDER::delivered();
    ::fold(digest, RENDER::sounded(clock), span);
  });
  out.digest = digest;
  return whole;
}

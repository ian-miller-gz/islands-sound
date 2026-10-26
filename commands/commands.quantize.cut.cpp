// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../inventory.hpp"
#include "../onset.hpp"
#include "../timeline.hpp"
#include "commands.quantize.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
using ARRANGEMENT::TRACK::LANE::Fade;

constexpr Whole HALVED = 2;

auto cuts(const Clip &clip, Float sense) -> Vector<Whole> {
  const TIMELINE::Derived source = TIMELINE::derived(clip.stock, clip);
  if (source.take == nullptr) return {};
  Vector<Whole> offsets;
  for (Whole at : TIMELINE::placed(clip, ONSET::found(*source.take, sense)))
    if (at > clip.at) offsets.push_back(at - clip.at);
  offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
  return offsets;
}

auto sourced(const Clip &clip, Whole opens) -> Whole {
  const Whole moved = Whole(std::lround(Float(opens) / clip.ratio));
  const Whole length = INVENTORY::length(INVENTORY::held().stocks[clip.stock]);
  return length == 0 ? clip.from + moved : (clip.from + moved) % length;
}

auto piece(const Clip &clip, Whole opens, Whole closes) -> Clip {
  Clip one = clip;
  one.at = clip.at + opens;
  one.from = ::sourced(clip, opens);
  one.frames = closes - opens;
  one.in = opens == 0 ? clip.in : Fade{};
  one.out = closes == clip.frames ? clip.out : Fade{};
  return one;
}

auto snapped(Whole at, Whole grid) -> Whole {
  return (at + grid / ::HALVED) / grid * grid;
}

void met(Vector<Clip> &pieces, const Clip &one) {
  while (!pieces.empty() && pieces.back().at >= one.at) pieces.pop_back();
  if (!pieces.empty())
    pieces.back().frames =
      std::min(pieces.back().frames, one.at - pieces.back().at);
  pieces.push_back(one);
}

}  // namespace

auto SOUND::COMMANDS::QUANTIZE::cut(const Clip &clip, Whole grid, Float sense)
  -> Vector<Clip> {
  const Vector<Whole> offsets = ::cuts(clip, sense);
  if (offsets.empty() || grid == 0) return {};
  Vector<Clip> pieces;
  Whole opens = 0;
  for (Whole at = 0; at <= offsets.size(); ++at) {
    const Whole closes = at < offsets.size() ? offsets[at] : clip.frames;
    Clip one = ::piece(clip, opens, closes);
    if (opens > 0) one.at = ::snapped(one.at, grid);
    ::met(pieces, one);
    opens = closes;
  }
  return pieces;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../control.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "render.governs.hpp"
#include "render.internal.hpp"
#include "render.rest.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;
using RENDER::REST::Spanned;

auto stood(const Vector<Spanned> &spans, const Spanned &one) -> Flag {
  return std::any_of(spans.begin(), spans.end(), [&](const Spanned &held) {
    return CONTROL::same(held.address, one.address) &&
           held.opens == one.opens && held.closes == one.closes;
  });
}

void curved(
  const Inventory &pool, const ARRANGEMENT::TRACK::Lane &lane,
  Vector<Spanned> &spans) {
  for (const Clip &span : lane.clips) {
    if (span.stock >= pool.stocks.size() || span.frames == 0) continue;
    const Curve &curve = pool.stocks[span.stock].curve;
    if (curve.points.empty()) continue;
    spans.push_back({curve.address, span.at, span.at + span.frames});
  }
}

void turned(
  const Inventory &pool, const ARRANGEMENT::TRACK::Lane &lane,
  const String &node, Vector<Spanned> &spans) {
  for (const Clip &span : lane.clips) {
    if (span.stock >= pool.stocks.size() || span.frames == 0) continue;
    for (const Turn &move : pool.stocks[span.stock].turns) {
      const Spanned one = {{node, move.number}, span.at, span.at + span.frames};
      if (!::stood(spans, one)) spans.push_back(one);
    }
  }
}

}  // namespace

auto SOUND::RENDER::REST::spanned() -> Vector<Spanned> {
  Vector<Spanned> spans;
  const Inventory &pool = INVENTORY::held();
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane) {
      const ARRANGEMENT::TRACK::Lane &held = tracks[track].lanes[lane];
      if (held.kind == KIND::LOGIC) ::curved(pool, held, spans);
      if (held.kind != KIND::CONTROL) continue;
      const String node = RENDER::governs(track, lane);
      if (!node.empty()) ::turned(pool, held, node, spans);
    }
  return spans;
}

auto SOUND::RENDER::REST::covered(
  const Vector<Spanned> &spans, const Address &address, Whole at) -> Flag {
  return std::any_of(spans.begin(), spans.end(), [&](const Spanned &one) {
    return CONTROL::same(one.address, address) && one.opens <= at &&
           at < one.closes;
  });
}

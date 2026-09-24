// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../shape.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;

auto between(Float opens, Float closes, Whole shape, Float part) -> Float {
  return opens + (closes - opens) * SHAPE::reading(shape, part);
}

auto travelled(Whole opens, Whole closes, Whole at) -> Float {
  return closes > opens ? Float(at - opens) / Float(closes - opens) : 1.0f;
}

void sampled(
  Vector<TIMELINE::Dialled> &run, const Curve &curve, Whole carried,
  Whole track, const ARRANGEMENT::TRACK::LANE::Clip &span, Whole from,
  Whole until, Whole grain) {
  if (curve.points.empty() || grain == 0) return;
  const Whole ends = std::min(span.at + span.frames, until);
  for (Whole at = from; at < ends; at += grain) {
    if (at < span.at) continue;
    run.push_back(
      {.track = track,
       .address = curve.address,
       .at = at,
       .value = TIMELINE::valued(
         curve, TIMELINE::wrapped(span.from, at - span.at, carried))});
  }
}

auto sooner(const TIMELINE::Dialled &one, const TIMELINE::Dialled &two)
  -> Flag {
  return one.at < two.at;
}

}  // namespace

auto SOUND::TIMELINE::valued(const Curve &curve, Whole at) -> Float {
  const Vector<Point> &points = curve.points;
  if (points.empty()) return 0.0f;
  Whole after = 0;
  while (after < points.size() && points[after].at <= at) ++after;
  if (after == 0) return points.front().value;
  if (after == points.size()) return points.back().value;
  const Point &one = points[after - 1];
  const Point &two = points[after];
  return ::between(
    one.value, two.value, one.shape, ::travelled(one.at, two.at, at));
}

auto SOUND::TIMELINE::valued(const Vector<Turn> &turns, Whole number, Whole at)
  -> Float {
  const Turn *one = nullptr;
  const Turn *two = nullptr;
  for (const Turn &move : turns) {
    if (move.number != number) continue;
    if (move.at > at)
      two = two == nullptr || move.at < two->at ? &move : two;
    else
      one = one == nullptr || move.at >= one->at ? &move : one;
  }
  if (one == nullptr) return two == nullptr ? 0.0f : two->value;
  if (two == nullptr) return one->value;
  return ::between(
    one->value, two->value, one->shape, ::travelled(one->at, two->at, at));
}

auto SOUND::TIMELINE::dial(
  const Inventory &pool, Whole from, Whole frames, Whole grain,
  const Vector<String> &claims) -> Vector<Dialled> {
  Vector<Dialled> run;
  const Whole until = from + frames;
  for (Whole index = 0; index < held().tracks.size(); ++index)
    for (const ARRANGEMENT::TRACK::Lane &lane : held().tracks[index].lanes) {
      if (lane.kind != KIND::LOGIC) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        if (
          span.stock < pool.stocks.size() &&
          claimed(claims, pool.stocks[span.stock].curve.address.node))
          ::sampled(
            run, pool.stocks[span.stock].curve,
            INVENTORY::length(pool.stocks[span.stock]), index, span, from,
            until, grain);
    }
  std::stable_sort(run.begin(), run.end(), ::sooner);
  return run;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

struct Kept {
  Whole track = NONE;
  Whole lane = 0;
  Whole number = 0;
  Float value = 0.0f;
};

auto moved(Vector<Kept> &kept, const TIMELINE::Sent &edge, Float value)
  -> Flag {
  for (Kept &stood : kept) {
    if (stood.track != edge.track || stood.lane != edge.lane) continue;
    if (stood.number != edge.number) continue;
    if (stood.value == value) return false;
    stood.value = value;
    return true;
  }
  kept.push_back(
    {.track = edge.track,
     .lane = edge.lane,
     .number = edge.number,
     .value = value});
  return true;
}

auto thinned(Vector<Kept> &kept, const TIMELINE::Sent &edge) -> Flag {
  if (edge.kind != KIND::CONTROL) return false;
  return !::moved(
    kept, edge, RENDER::worded(edge.track, edge.lane, edge.number, edge.value));
}

}  // namespace

auto SOUND::RENDER::sent(
  const Inventory &pool, Whole from, Whole frames,
  const Vector<String> &claims) -> Vector<TIMELINE::Sent> {
  const Vector<TIMELINE::Sent> run =
    TIMELINE::send(pool, from, frames, CONTROL::GRAIN, claims);
  Vector<::Kept> kept;
  Vector<TIMELINE::Sent> speaking;
  for (const TIMELINE::Sent &edge : run)
    if (!::thinned(kept, edge)) speaking.push_back(edge);
  return speaking;
}

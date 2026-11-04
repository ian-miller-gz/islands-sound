// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../kind.hpp"
#include "../transport.hpp"
#include "render.governs.hpp"
#include "render.internal.hpp"

auto SOUND::RENDER::edged(const TIMELINE::Sent &edge, Whole at)
  -> AUDIO::PLUGIN::Event {
  if (edge.kind == KIND::CONTROL)
    return {
      AUDIO::PLUGIN::Event::CONTROLLER, at, edge.number,
      worded(edge.track, edge.lane, edge.number, edge.value)};
  return {AUDIO::PLUGIN::Event::PROGRAM, at, edge.number, 0};
}

auto SOUND::RENDER::walk(Whole start, Whole frames) -> Walk {
  const Span cycle =
    TRANSPORT::held().looping ? TRANSPORT::cycled(start) : Span{};
  return walk(start, frames, cycle);
}

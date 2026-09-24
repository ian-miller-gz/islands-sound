// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "transport.internal.hpp"

namespace {

SOUND::TRANSPORT::Clockwork works;

}  // namespace

auto SOUND::TRANSPORT::clockwork() -> Clockwork & { return works; }

void SOUND::TRANSPORT::state() {
  const Span cycle = cycled();
  works.want.from = cycle.from;
  works.want.to = cycle.to;
  works.want.looping = works.setting.looping;
  works.deck.publish(works.want);
}

void SOUND::TRANSPORT::play() {
  works.want.playing = true;
  state();
}

void SOUND::TRANSPORT::stop() {
  works.want.playing = false;
  state();
}

void SOUND::TRANSPORT::locate(Whole frame) {
  works.want.mark = frame;
  ++works.want.locates;
  state();
}

auto SOUND::TRANSPORT::cycled() -> Span { return cycled(marker().position); }

auto SOUND::TRANSPORT::cycled(Whole at) -> Span {
  for (const Span &loop : works.setting.loops) {
    const Span span{
      SCORE::framed(loop.from, works.setting.tempos),
      SCORE::framed(loop.to, works.setting.tempos)};
    if (span.to > span.from && at >= span.from && at < span.to) return span;
  }
  return {};
}

void SOUND::TRANSPORT::tend() {
  const Span cycle = cycled();
  if (cycle.from != works.want.from || cycle.to != works.want.to) state();
}

auto SOUND::TRANSPORT::held() -> const Setting & { return works.setting; }

auto SOUND::TRANSPORT::mark() -> Whole { return works.want.mark; }

auto SOUND::TRANSPORT::seconds(Whole frames) -> Float {
  return static_cast<Float>(frames) / static_cast<Float>(RATE);
}

auto SOUND::TRANSPORT::marker() -> Marker {
  const Reading seen = works.mirror.read();
  const Flag reached = seen.locates == works.want.locates;
  return {
    reached ? seen.position : works.want.mark, works.want.playing,
    works.want.locates};
}

void SOUND::TRANSPORT::adopt(const Setting &setting) {
  works.setting = setting;
  for (Tempo &row : works.setting.tempos)
    row.tempo = std::clamp(row.tempo, SLOWEST, FASTEST);
  works.setting.looping = false;
  works.want.playing = false;
  works.want.mark = 0;
  ++works.want.locates;
  state();
}

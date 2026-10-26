// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>

#include "wave.overs.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot LEAF = "peak";
constexpr Float RAISED = 1.0f;
constexpr Whole LEAST = 1;

struct Bar {
  Float at = 0.0f, run = 0.0f;
  PEAKS::Peak peak;
};

VIEWS::Pool pool;

auto column(Whole at) -> String {
  return std::format("{}.{}.{}", WAVE::NAME, ::LEAF, at);
}

auto bar(const Vector<WAVE::Clip> &seated, Float at, Float step) -> Bar {
  const Whole frame = Whole(std::max(at, WAVE::ORIGIN));
  for (const WAVE::Clip &clip : seated) {
    if (frame < clip.at || frame >= clip.at + clip.frames) continue;
    const Whole run =
      std::min(std::max(Whole(step), ::LEAST), clip.at + clip.frames - frame);
    return {Float(frame), Float(run), WAVE::peak(clip, frame - clip.at, run)};
  }
  return {WAVE::ORIGIN, WAVE::ORIGIN, {}};
}

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::column(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::column(at);
    GUI::NODES::create(page, board.c_str(), WAVE::PANEL, id.c_str());
    WAVE::dressed(page, id, at);
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

void written(GUI::Handle page, Whole at, const Bar &bar, Float deep) {
  const String id = ::column(at);
  WAVE::dressed(page, id, at, bar.peak);
  const Float half = deep * WAVE::HALF;
  const Float rise = (bar.peak.most - bar.peak.least) * half;
  GUI::set(
    page, id.c_str(), GUI::Position{bar.at, half - bar.peak.most * half});
  GUI::set(page, id.c_str(), GUI::Extent{bar.run, std::max(rise, WAVE::HAIR)});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::peaks() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::peaks() {
  const GUI::Handle page = document();
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  const Float step = COLUMN / scale,
              west = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Whole count = Whole(window() / step) + 1;
  const Vector<Clip> seated = placed();
  Vector<::Bar> bars;
  for (Whole at = 0; at < count; ++at) {
    const ::Bar one = ::bar(seated, west + Float(at) * step, step);
    if (one.run > ORIGIN) bars.push_back(one);
  }
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, count, window());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::column, bars.size());
  const Float down = deep();
  for (Whole at = 0; at < bars.size(); ++at)
    ::written(page, at, bars[at], down);
}

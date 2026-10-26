// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot LINE = "wave onsets {}";
constexpr STRING::Hot INK = "onset";
constexpr Float RAISED = 2.0f;

VIEWS::Pool pool;

auto tick(Whole at) -> String { return std::format("{}.{}", WAVE::ONSET, at); }

auto shown(Float west, Float east) -> Vector<Whole> {
  const LANE::Face &face = LANE::faced(WAVE::laned());
  if (face.onsets == 0) return {};
  Vector<Whole> frames;
  for (const WAVE::Clip &clip : WAVE::placed())
    for (Whole at : WAVE::onsets(clip, face.sense))
      if (Float(at) >= west && Float(at) <= east) frames.push_back(at);
  return frames;
}

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::tick(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::tick(at);
    GUI::NODES::create(page, board.c_str(), WAVE::PANEL, id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::onsets() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::onsets() {
  const GUI::Handle page = document();
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Vector<Whole> frames = ::shown(west, west + window());
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, frames.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::tick, frames.size());
  const GUI::Extent run = {GUI::GET::extent(page, ONSET).w / scale, deep()};
  for (Whole at = 0; at < frames.size(); ++at) {
    const String id = ::tick(at);
    GUI::set(page, id.c_str(), GUI::Position{Float(frames[at]), ORIGIN});
    GUI::set(page, id.c_str(), run);
  }
}

void SOUND::VIEWS::WAVE::onsets(SHELL::Session &session) {
  if (LANE::faced(laned()).onsets == 0) return;
  session.print(std::format(::LINE, ::pool.lit));
}

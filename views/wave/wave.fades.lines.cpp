// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <iterator>

#include "../../boards.hpp"
#include "../../fade.hpp"
#include "../views.curves.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr Whole ENDS = std::size(FADE::ENDS);
constexpr STRING::Hot KIND = "stroke";
constexpr Float RAISED = 1.0f;

VIEWS::Pool pool;

auto named(Whole slot) -> String {
  return std::format(
    "{}.{}.{}", WAVE::LINED, slot / ::ENDS, FADE::ENDS[slot % ::ENDS].word);
}

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole slot = count; slot < ::pool.stood; ++slot)
    BOARDS::drop(page, ::named(slot));
  for (Whole slot = ::pool.stood; slot < count; ++slot) {
    const String id = ::named(slot);
    BOARDS::place(page, board.c_str(), ::KIND, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{WAVE::LINED});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

void written(GUI::Handle page, Whole slot, const WAVE::Clip &clip, Float deep) {
  const String id = ::named(slot);
  const FADE::End &end = FADE::ENDS[slot % ::ENDS];
  const FADE::Fade &fade = clip.*end.fade;
  const Whole frames = std::min(fade.frames, clip.frames);
  GUI::set(page, id.c_str(), GUI::Visibility{frames != 0});
  if (frames == 0) return;
  const Float edge = Float(clip.at + (end.east ? clip.frames : 0));
  const Float run = end.east ? -Float(frames) : Float(frames);
  const Float weight = GUI::GET::border(page, WAVE::LINED) / WAVE::scaled();
  VIEWS::stroke(page, id, {{edge, deep}, run, deep, fade.shape, weight});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::lines() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::lines() {
  const GUI::Handle page = document();
  if (scaled() <= ORIGIN) return;
  const Vector<Clip> seated = placed();
  VIEWS::reborn(::pool);
  const Whole wanted = seated.size() * ::ENDS;
  const Whole seats = VIEWS::sized(::pool, wanted, window());
  if (seats != ::pool.stood) ::build(page, seats);
  for (Whole slot = wanted; slot < ::pool.stood; ++slot)
    GUI::set(page, ::named(slot).c_str(), GUI::Visibility{false});
  const Float down = deep();
  for (Whole slot = 0; slot < std::min(wanted, ::pool.stood); ++slot)
    ::written(page, slot, seated[slot / ::ENDS], down);
}

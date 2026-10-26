// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "../../timeline.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot PATTERN = "waveshift";
constexpr STRING::Hot INK = "ink";
constexpr STRING::Hot WORD = "{:+g}";
constexpr Float RAISED = 3.0f;
constexpr Float INSET = 6.0f;
constexpr Float WIDE = 60.0f;
constexpr Float TALL = 16.0f;

VIEWS::Pool pool;

auto reading(Whole at) -> String { return std::format("{}.{}", ::PATTERN, at); }

void build(GUI::Handle page, Whole count) {
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, ::reading(at));
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::reading(at);
    BOARDS::place(page, WAVE::board().c_str(), WAVE::LABEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

auto worded(const WAVE::Clip &clip) -> String {
  return std::format(
    ::WORD, Float(clip.cents) / Float(TIMELINE::SHIFT::SEMITONE));
}

void written(GUI::Handle page, Whole at, const WAVE::Clip &clip, Float scale) {
  const String id = ::reading(at);
  const Flag shifted = clip.cents != WAVE::Clip{}.cents;
  GUI::set(page, id.c_str(), GUI::Visibility{shifted});
  if (!shifted) return;
  const Float north = WAVE::gripped().h + ::INSET;
  GUI::set(
    page, id.c_str(), GUI::Position{Float(clip.at) + ::INSET / scale, north});
  GUI::set(page, id.c_str(), GUI::Extent{::WIDE / scale, ::TALL});
  GUI::set(
    page, id.c_str(), GUI::Size{GUI::GET::size(page, ::PATTERN) / scale});
  GUI::set(page, id.c_str(), GUI::Text{::worded(clip)});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::shifts() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::shifts() {
  const GUI::Handle page = document();
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  const Vector<Clip> seated = placed();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, seated.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  for (Whole at = 0; at < ::pool.stood; ++at)
    ::written(page, at, at < seated.size() ? seated[at] : Clip{}, scale);
}

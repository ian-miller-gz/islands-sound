// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <iterator>

#include "../../boards.hpp"
#include "../../fade.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr Whole ENDS = std::size(FADE::ENDS);
constexpr STRING::Hot KIND = "node";
constexpr Float CROWDED = 4.0f;
constexpr Float RAISED = 2.0f;

VIEWS::Pool pool;

auto size() -> GUI::Extent {
  return GUI::GET::extent(VIEWS::document(), WAVE::GRIP);
}

auto named(Whole slot) -> String {
  return WAVE::grip(slot / ENDS, slot % ENDS);
}

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole slot = count; slot < ::pool.stood; ++slot)
    BOARDS::drop(page, ::named(slot));
  for (Whole slot = ::pool.stood; slot < count; ++slot) {
    const String id = ::named(slot);
    BOARDS::place(page, board.c_str(), ::KIND, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{WAVE::GRIP});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

void written(GUI::Handle page, Whole slot, const WAVE::Clip &clip) {
  const String id = ::named(slot);
  const Flag stands = WAVE::shown(clip);
  GUI::set(page, id.c_str(), GUI::Visibility{stands});
  if (!stands) return;
  const Float at = WAVE::west(clip, slot % ENDS);
  GUI::set(page, id.c_str(), GUI::Position{at, WAVE::ORIGIN});
  GUI::set(page, id.c_str(), WAVE::gripped());
}

}  // namespace

auto SOUND::VIEWS::WAVE::grip(Whole row, Whole end) -> String {
  return std::format("{}.{}.{}", GRIP, row, FADE::ENDS[end].word);
}

auto SOUND::VIEWS::WAVE::gripped() -> GUI::Extent {
  const GUI::Extent face = ::size();
  return {face.w / scaled(), face.h};
}

auto SOUND::VIEWS::WAVE::west(const Clip &clip, Whole end) -> Float {
  const FADE::End &side = FADE::ENDS[end];
  const Float wide = gripped().w;
  const Float knee = Float(clip.at + FADE::knee(clip, side));
  const Float least = Float(clip.at),
              most = Float(clip.at + clip.frames) - wide;
  return std::clamp(
    side.east ? knee - wide : knee, least, std::max(least, most));
}

auto SOUND::VIEWS::WAVE::shown(const Clip &clip) -> Flag {
  return Float(clip.frames) * scaled() >= ::size().w * ::CROWDED;
}

void SOUND::VIEWS::WAVE::SHED::grips() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::grips() {
  const GUI::Handle page = document();
  if (scaled() <= ORIGIN) return;
  const Vector<Clip> seated = placed();
  VIEWS::reborn(::pool);
  const Whole wanted = seated.size() * ::ENDS;
  const Whole seats = VIEWS::sized(::pool, wanted, window());
  if (seats != ::pool.stood) ::build(page, seats);
  for (Whole slot = wanted; slot < ::pool.stood; ++slot)
    GUI::set(page, ::named(slot).c_str(), GUI::Visibility{false});
  for (Whole slot = 0; slot < wanted; ++slot)
    ::written(page, slot, seated[slot / ::ENDS]);
}

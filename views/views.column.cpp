// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "trace/trace.face.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot COLUMN = "column";
constexpr STRING::Hot TRACKS = "tracks";
constexpr STRING::Hot UPPER = "tracks.body";
constexpr STRING::Hot ROSTER = "tracks.rows";
constexpr STRING::Hot TRACE = "trace";
constexpr STRING::Hot LOWER = "trace.body";
constexpr Float RIM = 1.0f;

auto least(GUI::Handle page, STRING::Hot rows) -> Float {
  return GUI::GET::position(page, rows).y + GUI::GET::pitch(page, rows) +
         GUI::GET::pad(page, rows) + GUI::GET::extent(page, rows).h;
}

void deepened(GUI::Handle page, STRING::Hot node, Float deep) {
  GUI::Extent size = GUI::GET::extent(page, node);
  size.h = deep;
  GUI::set(page, node, size);
}

void lowered(GUI::Handle page, STRING::Hot node, Float down) {
  GUI::Position at = GUI::GET::position(page, node);
  at.y = down;
  GUI::set(page, node, at);
}

}  // namespace

auto SOUND::VIEWS::split() -> Float {
  return GUI::GET::position(sheet(), BAR).y + GUI::GET::extent(sheet(), BAR).h +
         ::RIM;
}

void SOUND::VIEWS::column() {
  const GUI::Handle page = sheet();
  const Float deep = GUI::GET::measured(page, ::COLUMN).h;
  const Float bar = GUI::GET::extent(page, BAR).h;
  const Float low =
    GUI::GET::position(page, ::UPPER).y + ::least(page, ::ROSTER) + bar + ::RIM;
  const STRING::Hot list =
    VIEWS::TRACE::laning() ? VIEWS::TRACE::LANES : VIEWS::TRACE::ROWS;
  const Float high =
    deep - GUI::GET::position(page, ::LOWER).y - ::least(page, list);
  if (high < low) return;
  const Float place = std::clamp(split(), low, high);
  ::lowered(page, BAR, place - bar - ::RIM);
  ::deepened(page, ::TRACKS, place);
  ::lowered(page, ::TRACE, place);
  ::deepened(page, ::TRACE, deep - place);
}

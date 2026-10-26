// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "lane.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float ORIGIN = 0.0f;
constexpr Float HALF = 0.5f;

}  // namespace

void SOUND::VIEWS::LANE::slider() {
  const GUI::Handle page = document();
  const String id = slide();
  const String over = map();
  const Float scale = GUI::NGA::GET::zoom(page, over.c_str()).value;
  const Float wide = windowed();
  if (scale <= ::ORIGIN || wide <= ::ORIGIN) return;
  const Float west = GUI::NGA::GET::pan(page, over.c_str()).x;
  const Float here = GUI::NGA::GET::pan(page, field().c_str()).x;
  const GUI::Extent strip = GUI::GET::measured(page, over.c_str());
  const Float from = std::clamp((here - west) * scale, ::ORIGIN, strip.w);
  const Float to = std::clamp((here - west + wide) * scale, from, strip.w);
  GUI::set(page, id.c_str(), GUI::Position{from, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{to - from, strip.h});
}

void SOUND::VIEWS::LANE::strolled(GUI::Handle page, const String &id) {
  const GUI::NGA::Ask stroke = GUI::NGA::GET::stroked(page, id.c_str());
  if (stroke.board.empty()) return;
  wandered();
  const String board = field();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, board.c_str());
  GUI::NGA::set(
    page, board.c_str(), GUI::NGA::Pan{stroke.x - windowed() * ::HALF, held.y});
}

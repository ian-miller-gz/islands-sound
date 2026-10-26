// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "lane.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float ORIGIN = 0.0f;

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
  const Float deep = GUI::GET::measured(page, over.c_str()).h;
  GUI::set(page, id.c_str(), GUI::Position{(here - west) * scale, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{wide * scale, deep});
}

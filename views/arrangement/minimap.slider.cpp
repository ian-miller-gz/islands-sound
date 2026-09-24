// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot SLIDING = "slide";
constexpr Float ORIGIN = 0.0f;

auto slide() -> String {
  return String(VIEWS::ARRANGEMENT::map().substr(
           0, VIEWS::ARRANGEMENT::map().rfind('.'))) +
         "." + ::SLIDING;
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::slider() {
  const GUI::Handle page = document();
  const String id = ::slide();
  const String over = map();
  const Float scale = GUI::NGA::GET::zoom(page, over.c_str()).value;
  const Float wide = window();
  if (scale <= ::ORIGIN || wide <= ::ORIGIN) return;
  const Float west = GUI::NGA::GET::pan(page, over.c_str()).x;
  const Float field = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Float deep = GUI::GET::measured(page, over.c_str()).h;
  GUI::set(page, id.c_str(), GUI::Position{(field - west) * scale, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{wide * scale, deep});
}

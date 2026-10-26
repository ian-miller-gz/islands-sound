// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "signals.mapping.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUTTON = "lane.map";
constexpr STRING::Hot BAND = "signals.tools";
constexpr STRING::Hot PANEL = "lane.mapping";
constexpr STRING::Hot ROWS = "lane.mapping.rows";
constexpr Float WIDE = 220.0f;
constexpr Float FRAME = 12.0f;
constexpr Float AIR = 4.0f;
constexpr Whole WINDOW = 12;

void seated(GUI::Handle page) {
  const GUI::Position at = GUI::GET::origin(page, ::BUTTON);
  const GUI::Position home = GUI::GET::origin(page, ::BAND);
  const Float foot = GUI::GET::measured(page, ::BUTTON).h + ::AIR;
  GUI::set(page, ::PANEL, GUI::Position{0.0f, at.y - home.y + foot});
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::hung() -> Flag {
  return GUI::GET::visibility(document(), ::PANEL);
}

void SOUND::VIEWS::SIGNALS::hang(const Vector<Offer> &rows) {
  const GUI::Handle page = document();
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  const Float deep = Float(std::min(rows.size(), ::WINDOW)) * step;
  GUI::set(page, ::PANEL, GUI::Extent{::WIDE, ::FRAME + deep});
  ::seated(page);
  listing(::ROWS, rows);
}

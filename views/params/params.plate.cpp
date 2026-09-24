// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "params.internal.hpp"

namespace {
constexpr Float WIDE = 160.0f;
constexpr Float FRAME = 12.0f;
constexpr Whole WINDOW = 12;
}  // namespace

void SOUND::VIEWS::RACK::plate(GUI::Handle page, const Vector<String> &run) {
  const Float step = GUI::GET::pitch(page, WORDS) + GUI::GET::pad(page, WORDS);
  const Float deep = Float(std::min(run.size(), ::WINDOW)) * step;
  GUI::set(page, PLATE, GUI::Extent{::WIDE, ::FRAME + deep});
  GUI::set(page, WORDS, GUI::Rows{run.size()});
  const Whole first = GUI::GET::first(page, WORDS);
  for (Whole seat = 0; first + seat < run.size(); ++seat) {
    const String cell = std::format("{}.{}", WORDS, seat);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    GUI::set(page, cell.c_str(), GUI::Text{run[first + seat]});
  }
}

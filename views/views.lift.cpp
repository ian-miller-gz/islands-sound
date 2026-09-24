// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CHROME = "islands.splash";
constexpr STRING::Hot CHROMED = "shell";
constexpr Float MARGIN = 40.0f;
constexpr Float AIR = 4.0f;

constexpr STRING::Hot SEATS[] = {"views", "", "column", "main"};

Vector<Float> authored;
Float applied = 0.0f;

void read(GUI::Handle page) {
  if (!::authored.empty()) return;
  for (STRING::Hot seat : ::SEATS)
    ::authored.push_back(GUI::GET::position(page, seat).y);
}

auto lowered() -> Flag {
  const GUI::Handle chrome = GUI::GET::document(::CHROME);
  return chrome != GUI::NONE && !GUI::GET::visibility(chrome, ::CHROMED);
}

}  // namespace

void SOUND::VIEWS::lift() {
  const GUI::Handle page = sheet();
  const Float lift = ::lowered() ? ::MARGIN - ::AIR : 0.0f;
  ::read(page);
  if (lift == ::applied) return;
  for (Whole seat = 0; seat < ::authored.size(); ++seat) {
    const GUI::Position at = GUI::GET::position(page, ::SEATS[seat]);
    GUI::set(page, ::SEATS[seat], GUI::Position{at.x, ::authored[seat] - lift});
  }
  ::applied = lift;
}

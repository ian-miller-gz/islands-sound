// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../kind.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "arrangement.board.draft";
constexpr STRING::Hot PLATED = "plate";
constexpr Float HELD = 2.0f;

Flag stood = false;
Whole born = 0;

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::ghost() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) {
    BOARDS::place(page, board().c_str(), "panel", String(::STEM), {}, {});
    GUI::set(page, ::STEM, GUI::Depth{::HELD});
    ::stood = true;
  }
  const Draft drawing = drafting();
  const Flag showing = drawing.track != NONE && drawing.frames > 0;
  GUI::set(page, ::STEM, GUI::Visibility{showing});
  if (!showing) return;
  GUI::set(
    page, ::STEM, GUI::Position{across(drawing.at), inlay(drawing.band)});
  GUI::set(
    page, ::STEM, GUI::Extent{across(drawing.frames), inlaid(drawing.band)});
  GUI::set(
    page, ::STEM,
    GUI::Style{(String(::PLATED) + KIND::spoken(drawing.kind)).c_str()});
}

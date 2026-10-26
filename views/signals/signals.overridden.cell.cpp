// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "signals.overridden.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "sounding";
constexpr STRING::Hot LABEL = "label";
constexpr STRING::Hot INK = "ink";
constexpr Float LETTER = 12.0f;
constexpr Float MARGIN = 5.0f;
constexpr Float WIDTH = 26.0f;
constexpr Float WHOLE = 100.0f;
constexpr Whole FIGURES = 2;

Flag stood = false;
Whole born = 0;

auto id() -> String { return VIEWS::SIGNALS::column() + "." + ::STEM; }

void seat(GUI::Handle page) {
  const String cell = ::id();
  BOARDS::place(
    page, VIEWS::SIGNALS::column().c_str(), ::LABEL, cell, {},
    {::WIDTH, ::LETTER});
  GUI::set(page, cell.c_str(), GUI::Style{::INK});
  GUI::set(page, cell.c_str(), GUI::Size{::LETTER});
  ::stood = true;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::reading(Float value) -> String {
  return std::format("{:.{}f}", value, value >= ::WHOLE ? 0 : ::FIGURES);
}

void SOUND::VIEWS::SIGNALS::SHED::restore() {
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) return;
  BOARDS::drop(document(), ::id());
  ::stood = false;
}

void SOUND::VIEWS::SIGNALS::read(Flag on) {
  const GUI::Handle page = document();
  const String cell = ::id();
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) ::seat(page);
  GUI::set(page, cell.c_str(), GUI::Visibility{on});
  if (!on) return;
  const Float value = sounded();
  const Float north = GUI::NGA::GET::pan(page, board().c_str()).y;
  const Float line = (down(crossed(value)) - north) * sunk();
  GUI::set(page, cell.c_str(), GUI::Position{::MARGIN, line - ::LETTER});
  GUI::set(page, cell.c_str(), GUI::Text{reading(value)});
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../boards.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PANEL = "menu";
constexpr STRING::Hot ROWS = "menu.rows";
constexpr STRING::Hot ROOT = "root";
constexpr Float WIDE = 176.0f;
constexpr Float FRAME = 12.0f;
constexpr Float FOOT = 4.0f;
Vector<String> said;
String claim;
String hung;
GUI::Position ask;

auto depth(GUI::Handle page) -> Float {
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  return ::FRAME + Float(::said.size()) * step;
}

void worded(GUI::Handle page) {
  GUI::set(page, ::PANEL, GUI::Extent{::WIDE, ::depth(page)});
  GUI::set(page, ::ROWS, GUI::Rows{::said.size()});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < ::said.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    GUI::set(page, cell.c_str(), GUI::Text{::said[first + row]});
  }
}

void pointed(GUI::Handle page) {
  const GUI::Position home = GUI::GET::origin(page, ::ROOT);
  const GUI::Extent field = GUI::GET::measured(page, ::ROOT);
  Float east = ::ask.x - home.x, south = ::ask.y - home.y;
  if (field.w > 0.0f) east = std::min(east, field.w - ::WIDE);
  if (field.h > 0.0f) south = std::min(south, field.h - ::depth(page));
  GUI::set(
    page, ::PANEL, GUI::Position{std::max(0.0f, east), std::max(0.0f, south)});
}

}  // namespace

void SOUND::VIEWS::MENU::raise(
  const Vector<String> &rows, Float x, Float y, STRING::Hot whose) {
  const GUI::Handle page = document();
  ::said = rows;
  ::claim = whose;
  ::hung.clear();
  ::ask = {x, y};
  GUI::set(page, ::PANEL, GUI::Seat{});
  ::worded(page);
  ::pointed(page);
  GUI::set(page, ::PANEL, GUI::Visibility{true});
}

void SOUND::VIEWS::MENU::raise(
  const Vector<String> &rows, STRING::Hot whose, STRING::Hot door) {
  const GUI::Handle page = document();
  ::said = rows;
  ::claim = whose;
  ::hung = door;
  GUI::set(page, ::PANEL, GUI::Position{0.0f, 0.0f});
  GUI::set(page, ::PANEL, GUI::Seat{door, GUI::BELOW, ::FOOT});
  ::worded(page);
  GUI::set(page, ::PANEL, GUI::Visibility{true});
}

void SOUND::VIEWS::MENU::lower() {
  GUI::set(document(), ::PANEL, GUI::Visibility{false});
}

auto SOUND::VIEWS::MENU::standing() -> Flag {
  return GUI::GET::visibility(document(), ::PANEL);
}

auto SOUND::VIEWS::MENU::taken(STRING::Hot whose) -> Whole {
  const GUI::Handle page = document();
  if (!standing() || ::claim != whose) return NONE;
  if (!GUI::GET::activated(page, ::ROWS)) return NONE;
  GUI::set(page, ::PANEL, GUI::Visibility{false});
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  return row < ::said.size() ? row : NONE;
}

void SOUND::VIEWS::MENU::frame() {
  const GUI::Handle page = document();
  if (page == GUI::NONE || !standing()) return;
  ::worded(page);
  if (::hung.empty()) ::pointed(page);
  if (VIEWS::elsewhere(::hung.c_str(), ::PANEL)) lower();
}

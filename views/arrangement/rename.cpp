// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <island/gui/seat.hpp>

#include "../../inventory.hpp"
#include "arrangement.internal.hpp"
#include "keys.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot FIELD = "arrangement.rename";
constexpr STRING::Hot REGION = "arrangement";
constexpr STRING::Hot LABEL = ".label";

Whole stock = NONE;
String label;
Flag fresh = false;

void lowered(GUI::Handle page) {
  GUI::set(page, ::FIELD, GUI::Visibility{false});
  if (GUI::GET::editing(page) == ::FIELD) GUI::edit(page, "");
  if (!::label.empty()) GUI::set(page, ::label.c_str(), GUI::Visibility{true});
  ::stock = NONE;
  ::label.clear();
}

void seated(GUI::Handle page, Whole row) {
  const String board = VIEWS::ARRANGEMENT::board();
  ::label = GUI::SAC::SEAT::named(board.c_str(), row) + ::LABEL;
  const GUI::Position at = GUI::GET::origin(page, ::label.c_str());
  const GUI::Position corner = GUI::GET::origin(page, ::REGION);
  GUI::set(page, ::FIELD, GUI::Position{at.x - corner.x, at.y - corner.y});
  GUI::set(page, ::label.c_str(), GUI::Visibility{false});
}

void taken(GUI::Handle page) {
  const String name = GUI::GET::text(page, ::FIELD);
  if (!name.empty()) INVENTORY::name(::stock, name);
  ::lowered(page);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::rename(GUI::Handle page, Whole row) {
  if (row >= boxes().size()) return;
  if (!::label.empty()) ::lowered(page);
  const Inventory &pool = INVENTORY::held();
  const Whole held = boxes()[row].stock;
  if (held >= pool.stocks.size()) return;
  const String &name = pool.stocks[held].name;
  ::stock = held;
  ::fresh = true;
  ::seated(page, row);
  GUI::set(page, ::FIELD, GUI::Text{name});
  GUI::set(page, ::FIELD, GUI::Visibility{true});
  GUI::edit(page, ::FIELD);
  GUI::set(page, ::FIELD, GUI::Caret{name.size(), 0});
}

void SOUND::VIEWS::ARRANGEMENT::renaming(GUI::Handle page) {
  if (!GUI::GET::visibility(page, ::FIELD)) return;
  if (GUI::GET::committed(page, ::FIELD)) return ::taken(page);
  if (std::exchange(::fresh, false)) return;
  if (VIEWS::elsewhere("", ::FIELD)) ::lowered(page);
}

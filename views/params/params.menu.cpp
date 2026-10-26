// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "../views.menu.acts.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;
namespace RACK = SOUND::VIEWS::RACK;

constexpr STRING::Hot ROWS = "params.rows";
constexpr STRING::Hot RESETS = "Reset";
constexpr STRING::Hot TYPES = "Type";
constexpr STRING::Hot ARMS = "Arm";
constexpr STRING::Hot DISARMS = "Disarm";

auto stands(Whole n) -> Flag {
  const Vector<RACK::Row> rows = RACK::rows();
  return n < rows.size() && !RACK::head(rows[n]);
}

void reset(Whole n) {
  if (!::stands(n)) return;
  const RACK::Row row = RACK::rows()[n];
  RACK::moved(row, RACK::described(row).resting);
}

void type(Whole n) {
  const GUI::Handle page = VIEWS::document();
  const Whole first = GUI::GET::first(page, ::ROWS);
  if (!::stands(n) || n < first) return;
  const String cell = std::format("{}.{}.{}", ::ROWS, n - first, RACK::BARRED);
  if (GUI::edit(page, cell.c_str()) != 0) return;
  const Whole end = GUI::GET::text(page, cell.c_str()).size();
  GUI::set(page, cell.c_str(), GUI::Caret{.at = end, .from = 0});
}

auto idle(Whole n) -> Flag {
  if (!::stands(n)) return false;
  const RACK::Row row = RACK::rows()[n];
  return !RACK::listening(row) &&
         CONTROL::MAP::bound(RACK::addressed(row)).number == NONE;
}

auto engaged(Whole n) -> Flag { return ::stands(n) && !::idle(n); }

void arm(Whole n) {
  if (::stands(n)) CONTROL::MAP::arm(RACK::addressed(RACK::rows()[n]));
}

void disarm(Whole n) {
  if (::stands(n)) CONTROL::MAP::disarm(RACK::addressed(RACK::rows()[n]));
}

const Vector<VIEWS::MENU::Act> ACTS = {
  {.word = ::RESETS, .act = ::reset, .shown = ::stands},
  {.word = ::TYPES, .act = ::type, .shown = ::stands},
  {.word = ::ARMS, .act = ::arm, .shown = ::idle},
  {.word = ::DISARMS, .act = ::disarm, .shown = ::engaged}};

}  // namespace

void SOUND::VIEWS::RACK::menu(GUI::Handle page) {
  VIEWS::MENU::offer(page, ::ROWS, ::ACTS);
}

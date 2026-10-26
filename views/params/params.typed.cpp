// SPDX-License-Identifier: AGPL-3.0-or-later
#include "params.internal.hpp"

namespace {
using namespace SOUND;

void open(
  GUI::Handle page, const String &cell, const String &dial,
  const VIEWS::RACK::Row &row) {
  if (String(GUI::GET::editing(page)) != dial || !GUI::GET::seated(page))
    return;
  const String whole = VIEWS::RACK::shown(row);
  const String run = GUI::GET::text(page, dial.c_str());
  if (run == whole || run != VIEWS::RACK::fitted(page, cell, row)) return;
  GUI::set(page, dial.c_str(), GUI::Text{whole});
  GUI::set(page, dial.c_str(), GUI::Caret{.at = whole.size(), .from = 0});
}

}  // namespace

void SOUND::VIEWS::RACK::typed(
  GUI::Handle page, const String &cell, const Row &row) {
  const String dial = cell + "." + DIALLED;
  ::open(page, cell, dial, row);
  if (!GUI::GET::committed(page, dial.c_str())) return;
  Float value = 0.0f;
  if (RIG::parsed(described(row), GUI::GET::text(page, dial.c_str()), value))
    moved(row, value);
}

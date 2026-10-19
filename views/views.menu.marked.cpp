// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.menu.marked.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MARK = "✓ ";
constexpr STRING::Hot BARE = "  ";

auto marked(const Vector<String> &rows, const Vector<Flag> &ticks)
  -> Vector<String> {
  Vector<String> said;
  for (Whole at = 0; at < rows.size(); ++at) {
    const Flag ticked = at < ticks.size() && ticks[at];
    said.push_back(String(ticked ? ::MARK : ::BARE) + rows[at]);
  }
  return said;
}

auto marked(const Vector<String> &rows, Whole row) -> Vector<String> {
  Vector<Flag> ticks(rows.size(), false);
  if (row < ticks.size()) ticks[row] = true;
  return ::marked(rows, ticks);
}

}  // namespace

void SOUND::VIEWS::MENU::raise(
  const Vector<String> &rows, Float x, Float y, STRING::Hot whose, Whole row) {
  raise(::marked(rows, row), x, y, whose);
}

void SOUND::VIEWS::MENU::raise(
  const Vector<String> &rows, STRING::Hot whose, STRING::Hot door,
  const Vector<Flag> &ticks) {
  raise(::marked(rows, ticks), whose, door);
}

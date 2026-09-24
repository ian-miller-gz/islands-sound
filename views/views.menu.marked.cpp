// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MARK = "✓ ";
constexpr STRING::Hot BARE = "  ";

auto marked(const Vector<String> &rows, Whole row) -> Vector<String> {
  Vector<String> said;
  for (Whole at = 0; at < rows.size(); ++at)
    said.push_back(String(at == row ? ::MARK : ::BARE) + rows[at]);
  return said;
}

}  // namespace

void SOUND::VIEWS::MENU::raise(
  const Vector<String> &rows, Float x, Float y, STRING::Hot whose, Whole row) {
  raise(::marked(rows, row), x, y, whose);
}

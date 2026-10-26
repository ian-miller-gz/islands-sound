// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.internal.hpp"
#include "views.lit.hpp"

namespace {
using namespace SOUND;

Vector<VIEWS::LIT::Row> rows;

}  // namespace

void SOUND::VIEWS::LIT::clear() { ::rows.clear(); }

auto SOUND::VIEWS::lit() -> Vector<LIT::Row> {
  Vector<LIT::Row> shown;
  for (const LIT::Row &row : ::rows)
    if (String(row.style) != PLAIN) shown.push_back(row);
  return shown;
}

void SOUND::VIEWS::light(
  GUI::Handle page, const String &id, STRING::Hot style) {
  GUI::set(page, id.c_str(), GUI::Style{style});
  const auto at = std::ranges::find(::rows, id, &LIT::Row::id);
  if (at == ::rows.end())
    ::rows.push_back({id, style});
  else
    at->style = style;
}

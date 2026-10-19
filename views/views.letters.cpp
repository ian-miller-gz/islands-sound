// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.letters.hpp"

namespace {
constexpr STRING::Hot CUT = "…";
constexpr Whole MARKED = 1;
}  // namespace

auto SOUND::VIEWS::LETTERS::cropped(
  const String &word, Float wide, Float letter) -> String {
  const Whole columns = Whole(wide / (ADVANCE * letter));
  if (UNICODE::columns(word) <= columns) return word;
  if (columns <= ::MARKED) return {};
  String cut;
  Whole at = 0;
  while (UNICODE::columns(cut) + ::MARKED < columns)
    cut += UNICODE::encode(UNICODE::decode(word, at));
  return cut + String(::CUT);
}

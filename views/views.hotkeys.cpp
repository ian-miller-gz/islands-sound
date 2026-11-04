// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <iterator>
#include <string_view>

#include "views.hotkeys.hpp"

namespace {
using namespace SOUND;
using VIEWS::HOTKEYS::Row;
namespace KEYS = INPUT::KEYS;
namespace FAMILY = VIEWS::HOTKEYS::FAMILY;

constexpr STRING::Hot CONTROL = "Ctrl+";
constexpr STRING::Hot SHIFT = "Shift+";
constexpr STRING::Hot FUNCTION = "F";

struct Name {
  KEYS::Action action = KEYS::TEXT;
  STRING::Hot name = "";
};

constexpr Name NAMES[] = {{KEYS::ENTER, "Enter"},   {KEYS::ERASE, "Backspace"},
                          {KEYS::LEFT, "←"},        {KEYS::RIGHT, "→"},
                          {KEYS::UP, "↑"},          {KEYS::DOWN, "↓"},
                          {KEYS::HOME, "Home"},     {KEYS::END, "End"},
                          {KEYS::DELETE, "Delete"}, {KEYS::PRIOR, "PgUp"},
                          {KEYS::NEXT, "PgDn"}};

struct Code {
  Whole code = 0;
  STRING::Hot name = "";
};

constexpr Code CODES[] = {{KEYS::TAB, "Tab"}, {KEYS::ESCAPE, "Esc"}};

auto rows() -> Vector<Row> & {
  static Vector<Row> filed;
  return filed;
}

auto key(const VIEWS::Chord &chord) -> String {
  for (const Name &row : ::NAMES)
    if (row.action == chord.action) return row.name;
  if (chord.action == KEYS::HOTKEY)
    return std::format("{}{}", ::FUNCTION, chord.code);
  for (const Code &row : ::CODES)
    if (row.code == chord.code) return row.name;
  return String{char(chord.code)};
}

auto rank(STRING::Hot family) -> Whole {
  const auto *at = std::ranges::find_if(FAMILY::ORDER, [family](auto one) {
    return std::string_view(one) == family;
  });
  return Whole(std::distance(std::begin(FAMILY::ORDER), at));
}

auto before(const Row &one, const Row &two) -> Flag {
  const Whole first = ::rank(one.family), second = ::rank(two.family);
  if (first != second) return first < second;
  const std::string_view act = one.act, other = two.act;
  if (act != other) return act < other;
  return VIEWS::HOTKEYS::spelled(one.chord) <
         VIEWS::HOTKEYS::spelled(two.chord);
}

}  // namespace

auto SOUND::VIEWS::HOTKEYS::take(const Row &row) -> Flag {
  ::rows().push_back(row);
  return true;
}

auto SOUND::VIEWS::HOTKEYS::listed() -> Vector<Row> {
  Vector<Row> sorted = ::rows();
  std::ranges::sort(sorted, ::before);
  return sorted;
}

auto SOUND::VIEWS::HOTKEYS::spelled(const Chord &chord) -> String {
  String reading;
  if (chord.control) reading += ::CONTROL;
  if (chord.shift) reading += ::SHIFT;
  return reading + ::key(chord);
}

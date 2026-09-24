// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "../commands.hpp"
#include "views.hpp"

namespace {
using namespace SOUND;

constexpr Whole UNDO = 'Z';
constexpr Whole REDO = 'Y';
constexpr Whole CASED = 'a' - 'A';

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

}  // namespace

auto SOUND::VIEWS::journaled(const INPUT::KEYS::Event &key) -> Flag {
  if (key.action != INPUT::KEYS::TEXT || !key.control) return false;
  const Whole letter = ::folded(key.codepoint);
  if (letter == ::UNDO && key.shift) return !COMMANDS::redo().empty();
  if (letter == ::UNDO) return !COMMANDS::undo().empty();
  if (letter == ::REDO) return !COMMANDS::redo().empty();
  return false;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "arrangement.internal.hpp"
#include "arrangement.opened.hpp"
#include "keys.internal.hpp"

namespace {
using namespace SOUND;
namespace KEYS = INPUT::KEYS;

constexpr Whole COPY = 'C';
constexpr Whole PASTE = 'V';
constexpr Whole OPEN = 'E';
constexpr Whole SPLIT = 'S';
constexpr Whole RENAME = 'R';
constexpr Whole CASED = 'a' - 'A';

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

void axed(GUI::Handle) {
  VIEWS::ARRANGEMENT::lifted(VIEWS::ARRANGEMENT::chosen());
}

void edit(GUI::Handle page) {
  VIEWS::ARRANGEMENT::opened(page, VIEWS::ARRANGEMENT::chosen(page));
}

struct Act {
  KEYS::Action action = KEYS::TEXT;
  Whole letter = 0;
  Flag control = false;
  void (*act)(GUI::Handle page) = nullptr;
};

constexpr Act ACTS[] = {
  {.action = KEYS::DELETE, .act = ::axed},
  {.letter = ::COPY, .control = true, .act = VIEWS::ARRANGEMENT::copy},
  {.letter = ::PASTE, .control = true, .act = VIEWS::ARRANGEMENT::paste},
  {.letter = ::OPEN, .control = true, .act = ::edit},
  {.letter = ::SPLIT, .act = VIEWS::ARRANGEMENT::divide},
  {.letter = ::RENAME, .control = true, .act = VIEWS::ARRANGEMENT::rename},
};

auto matched(const Act &act, const KEYS::Event &key) -> Flag {
  if (act.action != key.action) return false;
  if (act.action != KEYS::TEXT) return true;
  return act.control == key.control && act.letter == ::folded(key.codepoint);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::keys() {
  const GUI::Handle page = VIEWS::document();
  opened(page);
  renaming(page);
  const KEYS::Event key = VIEWS::struck();
  if (VIEWS::journaled(key)) return;
  for (const ::Act &act : ::ACTS)
    if (::matched(act, key)) return act.act(page);
}

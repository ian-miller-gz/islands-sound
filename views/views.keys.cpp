// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <island/input.hpp>

#include "../commands.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole UNDO = 'Z';
constexpr Whole REDO = 'Y';
constexpr Whole CASED = 'a' - 'A';

INPUT::KEYS::Event held;
Flag drained = false;
Flag spent = false;
Flag deaf = false;
Flag hot = false;

auto matched(const VIEWS::Chord &chord, const INPUT::KEYS::Event &key) -> Flag {
  return key.action == chord.action &&
         VIEWS::folded(key.codepoint) == chord.code &&
         key.control == chord.control && key.shift == chord.shift;
}

auto real(const VIEWS::Chord &chord) -> Flag {
  if (chord.action != INPUT::KEYS::HOTKEY || chord.control || chord.shift)
    return false;
  const Flag keyed = ::held.action == INPUT::KEYS::HOTKEY;
  if (::hot || (keyed && ::held.codepoint == chord.code)) return false;
  return INPUT::GET::pressed(INPUT::KEYS::FUNCTION + chord.code);
}

struct Journal {
  VIEWS::Chord chord;
  auto (*act)() -> String = nullptr;
};

constexpr Journal JOURNALS[] = {
  {{.code = ::UNDO, .control = true}, COMMANDS::undo},
  {{.code = ::UNDO, .control = true, .shift = true}, COMMANDS::redo},
  {{.code = ::REDO, .control = true}, COMMANDS::redo}};

}  // namespace

auto SOUND::VIEWS::folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

auto SOUND::VIEWS::journaled(const INPUT::KEYS::Event &key) -> Flag {
  for (const ::Journal &row : ::JOURNALS)
    if (::matched(row.chord, key)) return !row.act().empty();
  return false;
}

auto SOUND::VIEWS::chorded(const Chord &chord) -> Flag {
  if (::deaf) return false;
  if (::real(chord)) {
    ::hot = true;
    return true;
  }
  if (::drained || !::matched(chord, ::held)) return false;
  ::drained = true;
  ::spent = true;
  return true;
}

void SOUND::VIEWS::polled() {
  GUI::poll();
  ::held = GUI::GET::keyed(sheet());
  ::drained = false;
  ::spent = false;
  ::deaf = false;
  ::hot = false;
}

auto SOUND::VIEWS::struck() -> INPUT::KEYS::Event {
  if (::deaf || std::exchange(::drained, true)) return {};
  return ::held;
}

void SOUND::VIEWS::listen(Flag on) { ::deaf = !on; }

auto SOUND::VIEWS::listening() -> Flag { return !::deaf; }

auto SOUND::VIEWS::escaped() -> Flag {
  if (::held.action != INPUT::KEYS::TEXT) return false;
  if (::held.codepoint != INPUT::KEYS::ESCAPE) return false;
  return !std::exchange(::spent, true);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "../views.hotkeys.hpp"
#include "piano.acts.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND::VIEWS::ROLL;
namespace KEYS = INPUT::KEYS;
namespace HOTKEYS = SOUND::VIEWS::HOTKEYS;

constexpr Whole COPY = 'C';
constexpr Whole PASTE = 'V';
constexpr Whole DRAW = 'D';
constexpr Whole PICK = 'S';
constexpr Whole ALL = 'A';
constexpr Whole SNAP = 'Q';
constexpr Whole INVERT = 'I';
constexpr Whole REVERSE = 'R';
constexpr Whole CASED = 'a' - 'A';
constexpr Integer FORTH = 1;
constexpr Integer BACK = -1;
constexpr STRING::Hot NUDGE = "nudge";
constexpr STRING::Hot DELETES = "delete";

void draw(GUI::Handle) { drawn(true); }

void pick(GUI::Handle) { drawn(false); }

template <Step step>
void nudge(GUI::Handle page) {
  moved(page, step);
}

constexpr auto RAISE = ::nudge<Step{.rows = ::FORTH}>;
constexpr auto LOWER = ::nudge<Step{.rows = ::BACK}>;
constexpr auto ABOVE = ::nudge<Step{.octaves = ::FORTH}>;
constexpr auto BELOW = ::nudge<Step{.octaves = ::BACK}>;
constexpr auto LATER = ::nudge<Step{.grains = ::FORTH}>;
constexpr auto EARLIER = ::nudge<Step{.grains = ::BACK}>;
constexpr auto AFTER = ::nudge<Step{.bars = ::FORTH}>;
constexpr auto BEFORE = ::nudge<Step{.bars = ::BACK}>;

constexpr Act ACTS[] = {
  {.letter = ::COPY, .control = true, .act = copy, .name = "copy"},
  {.letter = ::PASTE, .control = true, .act = paste, .name = "paste"},
  {.letter = ::ALL, .control = true, .act = all, .name = "all"},
  {.letter = ::DRAW, .act = ::draw, .name = "draw"},
  {.letter = ::PICK, .act = ::pick, .name = "select"},
  {.action = KEYS::DELETE, .act = plucked, .name = ::DELETES},
  {.action = KEYS::ERASE, .act = plucked, .name = ::DELETES},
  {.letter = KEYS::ESCAPE, .act = dropped, .name = "cancel"},
  {.action = KEYS::UP, .act = ::RAISE, .name = ::NUDGE},
  {.action = KEYS::DOWN, .act = ::LOWER, .name = ::NUDGE},
  {.action = KEYS::UP, .shift = true, .act = ::ABOVE, .name = ::NUDGE},
  {.action = KEYS::DOWN, .shift = true, .act = ::BELOW, .name = ::NUDGE},
  {.action = KEYS::RIGHT, .act = ::LATER, .name = ::NUDGE},
  {.action = KEYS::LEFT, .act = ::EARLIER, .name = ::NUDGE},
  {.action = KEYS::RIGHT, .shift = true, .act = ::AFTER, .name = ::NUDGE},
  {.action = KEYS::LEFT, .shift = true, .act = ::BEFORE, .name = ::NUDGE},
  {.letter = ::DRAW, .control = true, .act = doubled, .name = "duplicate"},
  {.letter = ::SNAP, .act = quantized, .name = "quantize"},
  {.letter = ::INVERT, .act = inverted, .name = "invert"},
  {.letter = ::REVERSE, .act = reversed, .name = "reverse"},
};

[[maybe_unused]] const Flag listed =
  HOTKEYS::offer(HOTKEYS::FAMILY::ROLL, ::ACTS);

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

auto matched(const Act &act, const KEYS::Event &key) -> Flag {
  if (act.action != key.action) return false;
  if (act.control != key.control || act.shift != key.shift) return false;
  if (act.action != KEYS::TEXT) return true;
  return act.letter == ::folded(key.codepoint);
}

}  // namespace

auto SOUND::VIEWS::ROLL::acted(GUI::Handle page, const INPUT::KEYS::Event &key)
  -> Flag {
  for (const Act &act : ::ACTS) {
    if (!::matched(act, key)) continue;
    act.act(page);
    return true;
  }
  return false;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/carry.hpp>
#include <island/gui/seat.hpp>
#include <island/input.hpp>

#include "piano.acts.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND::VIEWS::ROLL;
namespace KEYS = INPUT::KEYS;

constexpr Whole COPY = 'C';
constexpr Whole PASTE = 'V';
constexpr Whole DRAW = 'D';
constexpr Whole PICK = 'S';
constexpr Whole ALL = 'A';
constexpr Whole SNAP = 'Q';
constexpr Whole CASED = 'a' - 'A';
constexpr Integer FORTH = 1;
constexpr Integer BACK = -1;

void draw(GUI::Handle) { drawn(true); }

void pick(GUI::Handle) { drawn(false); }

template <Step step>
void nudge(GUI::Handle page) {
  moved(page, step);
}

constexpr Act ACTS[] = {
  {.action = KEYS::TEXT, .letter = ::COPY, .control = true, .act = copy},
  {.action = KEYS::TEXT, .letter = ::PASTE, .control = true, .act = paste},
  {.action = KEYS::TEXT, .letter = ::ALL, .control = true, .act = all},
  {.action = KEYS::TEXT, .letter = ::DRAW, .act = ::draw},
  {.action = KEYS::TEXT, .letter = ::PICK, .act = ::pick},
  {.action = KEYS::DELETE, .act = plucked},
  {.action = KEYS::ERASE, .act = plucked},
  {.action = KEYS::TEXT, .letter = KEYS::ESCAPE, .act = dropped},
  {.action = KEYS::UP, .act = ::nudge<Step{.rows = ::FORTH}>},
  {.action = KEYS::DOWN, .act = ::nudge<Step{.rows = ::BACK}>},
  {.action = KEYS::UP, .shift = true, .act = ::nudge<Step{.octaves = ::FORTH}>},
  {.action = KEYS::DOWN,
   .shift = true,
   .act = ::nudge<Step{.octaves = ::BACK}>},
  {.action = KEYS::RIGHT, .act = ::nudge<Step{.grains = ::FORTH}>},
  {.action = KEYS::LEFT, .act = ::nudge<Step{.grains = ::BACK}>},
  {.action = KEYS::RIGHT, .shift = true, .act = ::nudge<Step{.bars = ::FORTH}>},
  {.action = KEYS::LEFT, .shift = true, .act = ::nudge<Step{.bars = ::BACK}>},
  {.action = KEYS::TEXT, .letter = ::DRAW, .control = true, .act = doubled},
  {.action = KEYS::TEXT, .letter = ::SNAP, .act = quantized},
};

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

void SOUND::VIEWS::ROLL::all(GUI::Handle page) {
  const String board = VIEWS::ROLL::board();
  const Vector<Pip> &shown = plates();
  Vector<String> marks;
  for (Whole row = 0; row < shown.size(); ++row)
    if (shown[row].stock != NONE && shown[row].index != NONE)
      marks.push_back(GUI::SAC::SEAT::named(board.c_str(), row));
  GUI::SAC::CARRY::follow(page, marks);
}

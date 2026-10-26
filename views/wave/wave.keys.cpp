// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "../views.quantize.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace KEYS = INPUT::KEYS;

constexpr Whole SNAP = 'Q';
constexpr Whole CASED = 'a' - 'A';

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

auto snapping(const KEYS::Event &key) -> Flag {
  return key.action == KEYS::TEXT && !key.control && !key.shift &&
         ::folded(key.codepoint) == ::SNAP;
}

}  // namespace

void SOUND::VIEWS::WAVE::keyed() {
  const KEYS::Event key = VIEWS::struck();
  if (VIEWS::journaled(key) || !::snapping(key)) return;
  const LANE::Mark mark = laned();
  const Whole row = chosen();
  if (mark.lane == NONE || row == NONE) return;
  VIEWS::quantized(mark.track, mark.lane, row, LANE::faced(mark).sense);
}

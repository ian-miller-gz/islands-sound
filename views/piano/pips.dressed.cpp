// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "piano.notation.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NOTE = "note";
constexpr STRING::Hot LOOSE = "loose";
constexpr STRING::Hot HIT = "hit";
constexpr STRING::Hot STYLES[][2] = {{::NOTE, ::LOOSE}, {::HIT, ::HIT}};

Whole worn = NONE;
Vector<Whole> tiers;

auto loosely(const VIEWS::ROLL::Pip &pip) -> Flag { return pip.stock == NONE; }

auto styled(Whole notation, Flag loose, Whole tier) -> String {
  const STRING::Hot stem = ::STYLES[notation][loose];
  if (notation != VIEWS::ROLL::TONAL || loose) return stem;
  return VIEWS::ROLL::tinted(stem, tier);
}

}  // namespace

void SOUND::VIEWS::ROLL::dressed(
  GUI::Handle page, const Vector<Pip> &wanted, const Vector<Pip> &shown) {
  const String stem = board();
  const Whole notation = GET::notation();
  const Flag again = notation != ::worn;
  ::worn = notation;
  ::tiers.resize(wanted.size(), NONE);
  for (Whole row = 0; row < wanted.size(); ++row) {
    const Flag loose = ::loosely(wanted[row]);
    const Whole now = tier(wanted[row]);
    const Flag kept = row < shown.size() && ::loosely(shown[row]) == loose &&
                      ::tiers[row] == now;
    ::tiers[row] = now;
    if (!again && kept) continue;
    const String id = GUI::SAC::SEAT::named(stem.c_str(), row);
    const String style = ::styled(notation, loose, now);
    GUI::set(page, id.c_str(), GUI::Style{style.c_str()});
    GUI::set(page, id.c_str(), GUI::Depth{loose ? LOOSE : SEATED});
  }
}

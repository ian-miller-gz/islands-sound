// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "piano.notation.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace HEADS = SOUND::VIEWS::ROLL::HEADS;

Vector<HEADS::Worn> worn;
GUI::NGA::Zoom zoomed;
Whole born = 0;

auto inked(const VIEWS::ROLL::Pip &pip, const Vector<Whole> &chosen, Whole row)
  -> Whole {
  if (pip.stock == NONE) return HEADS::AFLOAT;
  return std::ranges::binary_search(chosen, row) ? HEADS::CHOSEN
                                                 : HEADS::RESTING;
}

constexpr Whole HALF_NOTE = 2 * PULSES;

auto facts(
  const VIEWS::ROLL::Pip &pip, const Vector<Whole> &chosen, Whole row,
  Whole notation, Float side, Flag flipped) -> HEADS::Worn {
  if (!VIEWS::ROLL::GET::staffed())
    return {NONE, NONE, notation, NONE, NONE, VIEWS::ROLL::STAFF::NOWHERE};
  const Flag pitched = VIEWS::ROLL::GET::pitched();
  return {
    pip.pitch,
    ::inked(pip, chosen, row),
    notation,
    VIEWS::ROLL::tier(pip),
    HEADS::banded(pip),
    VIEWS::ROLL::STAFF::placed(pip.pitch),
    side,
    flipped,
    pitched ? VIEWS::ROLL::STAFF::accidental(pip.pitch) : NONE,
    pitched && pip.length >= ::HALF_NOTE,
    pitched};
}

auto rezoomed(GUI::Handle page) -> Flag {
  const GUI::NGA::Zoom zoom =
    GUI::NGA::GET::zoom(page, VIEWS::ROLL::board().c_str());
  const Flag moved = zoom.value != ::zoomed.value || zoom.down != ::zoomed.down;
  ::zoomed = zoom;
  return moved;
}

}  // namespace

void SOUND::VIEWS::ROLL::HEADS::seat(
  GUI::Handle page, const Vector<Pip> &wanted) {
  if (VIEWS::reborn(::born)) ::worn.clear();
  const Whole notation = GET::notation();
  if (!GET::staffed() && ::worn.empty()) return;
  if (::worn.size() > wanted.size()) ::worn.resize(wanted.size());
  const Vector<Whole> rows = chosen(page);
  const Float edge = side(wanted);
  const Flag again = ::rezoomed(page) && GET::staffed();
  const Vector<Flag> flips = flipped(wanted);
  for (Whole row = 0; row < wanted.size(); ++row) {
    if (row == ::worn.size()) {
      stand(page, row);
      ::worn.push_back({});
    }
    const Worn now =
      ::facts(wanted[row], rows, row, notation, edge, flips[row]);
    if (!again && now == ::worn[row]) continue;
    dress(page, row, now);
    ::worn[row] = now;
  }
}

void SOUND::VIEWS::ROLL::SHED::heads() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::worn.clear();
  for (Whole row = 0; row < ::worn.size(); ++row) HEADS::drop(page, row);
  ::worn.clear();
}

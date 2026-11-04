// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "piano.acts.hpp"

namespace {
using namespace SOUND;

constexpr Float STILL = 0.0f;
constexpr Integer HALVED = 2;

struct Window {
  Integer opens = 0, closes = 0;
};

auto window(GUI::Handle page) -> Window {
  const Float grain = Float(VIEWS::ROLL::GRAIN);
  const Float west =
    GUI::NGA::GET::pan(page, VIEWS::ROLL::board().c_str()).x;
  const Float from = west > ::STILL ? west : ::STILL;
  return {
    Integer(from * grain),
    Integer((from + VIEWS::ROLL::window().w) * grain)};
}

}  // namespace

void SOUND::VIEWS::ROLL::centred(GUI::Handle page, Vector<Loose> &notes) {
  if (notes.empty()) return;
  const ::Window seen = ::window(page);
  if (seen.closes <= seen.opens) return;
  Integer first = Integer(notes.front().at), last = first;
  for (const Loose &one : notes) {
    first = std::min(first, Integer(one.at));
    last = std::max(last, Integer(one.at + one.length));
  }
  if (first >= seen.opens && first < seen.closes) return;
  const Integer centre = (seen.opens + seen.closes) / ::HALVED;
  const Integer shift = centre - (first + last) / ::HALVED;
  for (Loose &one : notes)
    one.at = Whole(std::max<Integer>(Integer(one.at) + shift, 0));
}

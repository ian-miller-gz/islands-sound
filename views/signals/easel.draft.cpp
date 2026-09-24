// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../../shape.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

Vector<VIEWS::SIGNALS::Point> drafting;
Flag spanning = false;

constexpr STRING::Hot DREW = "draw";

auto owned(Whole at) -> Whole {
  for (Whole row = 0; row < ::drafting.size(); ++row)
    if (::drafting[row].at == at) return row;
  return NONE;
}

void lay(Integer across, Float value, Whole shape) {
  const Whole at = VIEWS::SIGNALS::stamped(across);
  if (VIEWS::SIGNALS::aimed(at).stock == NONE) return;
  const Whole row = ::owned(at);
  if (row == NONE)
    return ::drafting.push_back(
      {VIEWS::SIGNALS::aimed(at).stock, NONE, at, at + VIEWS::SIGNALS::grain(),
       value, shape});
  ::drafting[row].value = value;
  ::drafting[row].shape = shape;
}

auto standing(Whole at) -> Whole {
  const VIEWS::SIGNALS::Aim aim = VIEWS::SIGNALS::aimed(at);
  if (aim.stock == NONE) return SHAPE::HOLD;
  const Whole held =
    VIEWS::SIGNALS::covered(aim.stock, VIEWS::SIGNALS::inside(aim, at));
  return held == NONE ? Whole(SHAPE::HOLD)
                      : VIEWS::SIGNALS::row(aim.stock, held).shape;
}

void lay(const GUI::SAC::STROKE::Cell &cell) {
  ::lay(cell.across, VIEWS::SIGNALS::valued(cell.down), SHAPE::LINE);
}

void cleared() {
  if (!::spanning || ::drafting.size() < 2) return;
  const VIEWS::SIGNALS::Aim aim = VIEWS::SIGNALS::aimed(::drafting.front().at);
  const VIEWS::SIGNALS::Aim far = VIEWS::SIGNALS::aimed(::drafting.back().at);
  if (aim.stock == NONE || far.stock != aim.stock) return;
  const Whole opens = VIEWS::SIGNALS::inside(aim, ::drafting.front().at);
  const Whole closes = VIEWS::SIGNALS::inside(far, ::drafting.back().at);
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  VIEWS::SIGNALS::clear(
    aim.stock, std::min(opens, closes), std::max(opens, closes));
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::stamped(Integer across) -> Whole {
  return across <= 0 ? 0 : Whole(across) * grain();
}

void SOUND::VIEWS::SIGNALS::begun() {
  ::drafting.clear();
  ::spanning = false;
}

void SOUND::VIEWS::SIGNALS::drawn(const Vector<GUI::SAC::STROKE::Cell> &cells) {
  for (const GUI::SAC::STROKE::Cell &cell : cells) ::lay(cell);
}

void SOUND::VIEWS::SIGNALS::spanned(
  const GUI::SAC::STROKE::Cell &from, const GUI::SAC::STROKE::Cell &to,
  Whole shape) {
  ::drafting.clear();
  ::spanning = true;
  ::lay(from.across, valued(from.down), shape);
  if (stamped(to.across) == stamped(from.across)) return;
  ::lay(to.across, valued(to.down), ::standing(stamped(to.across)));
  if (::drafting.size() == 2) ::drafting.front().until = ::drafting.back().at;
}

auto SOUND::VIEWS::SIGNALS::landed() -> Whole {
  Whole laid = 0;
  if (!::spanning) {
    thinned(::drafting);
    ::spanning = ::drafting.size() > 1;
  }
  HISTORY::begin();
  ::cleared();
  for (const Point &point : ::drafting) {
    const Aim aim = aimed(point.at);
    if (aim.stock == NONE) continue;
    const Row laying = {inside(aim, point.at), point.value, point.shape};
    const Whole held = covered(aim.stock, laying.at);
    HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
    if (held != NONE) INVENTORY::erase(aim.stock, held);
    if (write(aim.stock, laying)) ++laid;
    HISTORY::wrote(::DREW, aim.stock, INVENTORY::held().stocks[aim.stock]);
  }
  begun();
  HISTORY::end();
  return laid;
}

void SOUND::VIEWS::SIGNALS::drafted(Vector<Point> &wanted) {
  wanted.insert(wanted.end(), ::drafting.begin(), ::drafting.end());
}

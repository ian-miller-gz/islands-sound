// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../history.hpp"
#include "../../shape.hpp"
#include "easel.draft.hpp"

namespace {
using namespace SOUND;

Vector<VIEWS::SIGNALS::Point> drafting;
Flag spanning = false;
Flag arced = false;

auto owned(Whole at) -> Whole {
  for (Whole row = 0; row < ::drafting.size(); ++row)
    if (::drafting[row].at == at) return row;
  return NONE;
}

auto claimed(Whole at) -> Whole {
  const Whole stood = VIEWS::SIGNALS::aimed(at).stock;
  if (stood != NONE) return stood;
  if (!::drafting.empty()) {
    const VIEWS::SIGNALS::Aim first =
      VIEWS::SIGNALS::aimed(::drafting.front().at);
    if (VIEWS::SIGNALS::extended(first, at))
      return VIEWS::SIGNALS::aimed(at).stock;
  }
  return VIEWS::SIGNALS::claimed(at).stock;
}

void lay(const GUI::SAC::STROKE::Cell &cell) {
  VIEWS::SIGNALS::DRAFT::lay(
    cell.across, VIEWS::SIGNALS::valued(cell.down), SHAPE::LINE);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::DRAFT::points() -> Vector<Point> & {
  return ::drafting;
}

auto SOUND::VIEWS::SIGNALS::DRAFT::spanning() -> Flag & { return ::spanning; }

void SOUND::VIEWS::SIGNALS::DRAFT::lay(
  Integer across, Float value, Whole shape) {
  const Whole at = stamped(across);
  if (!::arced) {
    HISTORY::begin();
    ::arced = true;
  }
  const Whole stock = ::claimed(at);
  if (stock == NONE) return;
  const Whole row = ::owned(at);
  if (row == NONE)
    return ::drafting.push_back({stock, NONE, at, at + grain(), value, shape});
  ::drafting[row].value = value;
  ::drafting[row].shape = shape;
}

auto SOUND::VIEWS::SIGNALS::DRAFT::standing(Whole at) -> Whole {
  const Aim aim = aimed(at);
  if (aim.stock == NONE) return SHAPE::HOLD;
  const Whole held = covered(aim.stock, inside(aim, at));
  return held == NONE ? Whole(SHAPE::HOLD) : row(aim.stock, held).shape;
}

auto SOUND::VIEWS::SIGNALS::stamped(Integer across) -> Whole {
  return across <= 0 ? 0 : Whole(across) * grain();
}

void SOUND::VIEWS::SIGNALS::begun() {
  ::drafting.clear();
  ::spanning = false;
  if (!::arced) return;
  ::arced = false;
  HISTORY::end();
}

void SOUND::VIEWS::SIGNALS::drawn(const Vector<GUI::SAC::STROKE::Cell> &cells) {
  for (const GUI::SAC::STROKE::Cell &cell : cells) ::lay(cell);
}

void SOUND::VIEWS::SIGNALS::drafted(Vector<Point> &wanted) {
  wanted.insert(wanted.end(), ::drafting.begin(), ::drafting.end());
}

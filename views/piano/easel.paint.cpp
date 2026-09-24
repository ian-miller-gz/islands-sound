// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ADRIFT = "no notes lane on the chosen track";
constexpr STRING::Hot PAINTED = "paint";

struct Cell {
  Integer at = 0, pitch = 0;
};

Cell opened;
Flag rubbing = false;
Cell last;
Flag written = false;

void said(GUI::Handle page, STRING::Hot text) {
  GUI::set(page, VIEWS::ROLL::NOTICE, GUI::Text{text});
}

auto celled(const GUI::SAC::STROKE::Cell &cell) -> Cell {
  const Integer top = Integer(VIEWS::ROLL::HIGHEST);
  if (cell.down <= 0) return {cell.across, top};
  return {cell.across, cell.down < top ? top - cell.down : 0};
}

auto held(const Score &score, const Cell &cell, Whole at, Whole span) -> Whole {
  for (Whole index = 0; index < score.notes.size(); ++index) {
    const Note &note = score.notes[index];
    if (
      Integer(note.pitch) == cell.pitch && note.at < at + span &&
      at < note.at + note.length)
      return index;
  }
  return NONE;
}

void paint(GUI::Handle page, const Cell &fell) {
  const Whole span = VIEWS::ROLL::grained();
  const Whole at = Whole(fell.at) * span;
  const Cell cell = {
    fell.at, Integer(VIEWS::ROLL::degreed(Whole(fell.pitch), at))};
  const VIEWS::ROLL::Aim aim = VIEWS::ROLL::claimed(at);
  if (aim.stock == NONE) return ::said(page, ::ADRIFT);
  const Whole into = VIEWS::ROLL::inside(aim, at);
  Score score = INVENTORY::held().stocks[aim.stock].score;
  const Flag corner = ::written && ::last.at == cell.at;
  if (corner && ::last.pitch == cell.pitch) return;
  if (::held(score, cell, into, span) != NONE) return;
  if (corner) {
    const Whole old = ::held(score, ::last, into, span);
    if (old != NONE) SCORE::erase(score, old);
  }
  SCORE::write(
    score, {.pitch = Whole(cell.pitch),
            .at = into,
            .length = span,
            .velocity = VIEWS::ROLL::bench().loud / VIEWS::ROLL::FULL});
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  INVENTORY::score(aim.stock, score);
  HISTORY::wrote(::PAINTED, aim.stock, INVENTORY::held().stocks[aim.stock]);
  ::last = cell;
  ::written = true;
  ::said(page, "");
}

}  // namespace

auto SOUND::VIEWS::ROLL::grained() -> Whole {
  return bench().snap > GRAIN ? bench().snap : GRAIN;
}

void SOUND::VIEWS::ROLL::begun(
  GUI::Handle page, const GUI::SAC::STROKE::Cell &cell) {
  ::written = false;
  ::rubbing = false;
  ::opened = ::celled(cell);
  const Whole span = VIEWS::ROLL::grained();
  const Whole at = Whole(::opened.at) * span;
  const VIEWS::ROLL::Aim aim =
    VIEWS::ROLL::aimed(SCORE::framed(at, TRANSPORT::held().tempos));
  if (aim.stock == NONE) return;
  const Score &score = INVENTORY::held().stocks[aim.stock].score;
  ::rubbing =
    ::held(score, ::opened, VIEWS::ROLL::inside(aim, at), span) != NONE;
}

void SOUND::VIEWS::ROLL::painted(
  GUI::Handle page, const Vector<GUI::SAC::STROKE::Cell> &cells) {
  for (const GUI::SAC::STROKE::Cell &cell : cells)
    ::paint(page, ::celled(cell));
}

auto SOUND::VIEWS::ROLL::ended(
  GUI::Handle page, const GUI::SAC::STROKE::Cell &cell, Flag dragged) -> Flag {
  if (!::rubbing || dragged) return false;
  const Whole span = VIEWS::ROLL::grained();
  const Whole at = Whole(::opened.at) * span;
  const VIEWS::ROLL::Aim aim =
    VIEWS::ROLL::aimed(SCORE::framed(at, TRANSPORT::held().tempos));
  if (aim.stock == NONE) return true;
  Score score = INVENTORY::held().stocks[aim.stock].score;
  const Whole index =
    ::held(score, ::opened, VIEWS::ROLL::inside(aim, at), span);
  if (index == NONE) return true;
  const Cell to = ::celled(cell);
  const Whole under =
    ::held(score, to, VIEWS::ROLL::inside(aim, Whole(to.at) * span), span);
  if (under != index) return true;
  SCORE::erase(score, index);
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  INVENTORY::score(aim.stock, score);
  HISTORY::wrote(::PAINTED, aim.stock, INVENTORY::held().stocks[aim.stock]);
  ::said(page, "");
  return true;
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "piano.easel.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace EASEL = SOUND::VIEWS::ROLL::EASEL;
using EASEL::Cell;

constexpr STRING::Hot ADRIFT = "track lacks notes lane";

Cell last;
Flag written = false;

void ledgered(const ROLL::Aim &aim, const Score &score) {
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  INVENTORY::score(aim.stock, score);
  HISTORY::wrote(
    EASEL::PAINTED, aim.stock, INVENTORY::held().stocks[aim.stock]);
}

void paint(GUI::Handle page, const Cell &fell) {
  if (Whole(fell.pitch) == NONE) return;
  const Whole span = ROLL::grained();
  const Whole at = Whole(fell.at) * span;
  const Cell cell = {fell.at, Integer(ROLL::degreed(Whole(fell.pitch), at))};
  const ROLL::Aim aim = ROLL::claimed(at);
  if (aim.stock == NONE) return EASEL::said(page, ::ADRIFT);
  const Whole into = ROLL::inside(aim, at);
  Score score = INVENTORY::held().stocks[aim.stock].score;
  const Flag corner = ::written && ::last.at == cell.at;
  if (corner && ::last.pitch == cell.pitch) return;
  if (EASEL::held(score, cell, into, span) != NONE) return;
  const Whole old = corner ? EASEL::held(score, ::last, into, span) : NONE;
  if (old != NONE) SCORE::erase(score, old);
  SCORE::write(
    score, {.pitch = Whole(cell.pitch),
            .at = into,
            .length = span,
            .velocity = ROLL::bench().loud / ROLL::FULL});
  ::ledgered(aim, score);
  ::last = cell;
  ::written = true;
  EASEL::said(page, "");
}

}  // namespace

void SOUND::VIEWS::ROLL::EASEL::said(GUI::Handle page, STRING::Hot text) {
  GUI::set(page, NOTICE, GUI::Text{text});
}

auto SOUND::VIEWS::ROLL::EASEL::celled(const GUI::SAC::STROKE::Cell &cell)
  -> Cell {
  const Whole at = cell.across > 0 ? Whole(cell.across) * grained() : 0;
  return {cell.across, Integer(ROWS::pitch(Float(cell.down) * STEP, at))};
}

auto SOUND::VIEWS::ROLL::EASEL::held(
  const Score &score, const Cell &cell, Whole at, Whole span) -> Whole {
  for (Whole index = 0; index < score.notes.size(); ++index) {
    const Note &note = score.notes[index];
    if (
      Integer(note.pitch) == cell.pitch && note.at < at + span &&
      at < note.at + note.length)
      return index;
  }
  return NONE;
}

auto SOUND::VIEWS::ROLL::grained() -> Whole {
  return bench().snap > GRAIN ? bench().snap : GRAIN;
}

void SOUND::VIEWS::ROLL::begun(
  GUI::Handle page, const GUI::SAC::STROKE::Cell &cell) {
  ::written = false;
  EASEL::open(EASEL::celled(cell));
}

void SOUND::VIEWS::ROLL::painted(
  GUI::Handle page, const Vector<GUI::SAC::STROKE::Cell> &cells) {
  for (const GUI::SAC::STROKE::Cell &cell : cells)
    ::paint(page, EASEL::celled(cell));
}

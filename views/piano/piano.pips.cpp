// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/carry.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LOOSE = "loose";

void gathered(
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole track,
  Vector<VIEWS::ROLL::Pip> &wanted) {
  const Inventory &pool = INVENTORY::held();
  if (span.stock >= pool.stocks.size()) return;
  const Score &score = pool.stocks[span.stock].score;
  const Whole opens = VIEWS::ROLL::opened(span.at);
  const Whole held = VIEWS::ROLL::opened(span.frames);
  for (Whole index = 0; index < score.notes.size(); ++index) {
    const Note &note = score.notes[index];
    if (note.at < span.from || note.at - span.from >= held) continue;
    wanted.push_back(
      {span.stock, index, note.pitch, opens + note.at - span.from, note.length,
       track});
  }
}

auto loosely(const VIEWS::ROLL::Pip &pip) -> Flag { return pip.stock == NONE; }

void dressed(
  GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted,
  const Vector<VIEWS::ROLL::Pip> &shown) {
  const String board = VIEWS::ROLL::board();
  for (Whole row = 0; row < wanted.size(); ++row) {
    const Flag loose = ::loosely(wanted[row]);
    if (row < shown.size() && ::loosely(shown[row]) == loose) continue;
    const String id = GUI::SAC::SEAT::named(board.c_str(), row);
    GUI::set(page, id.c_str(), GUI::Style{loose ? ::LOOSE : BOARDS::PLATE});
    GUI::set(
      page, id.c_str(),
      GUI::Depth{loose ? VIEWS::ROLL::LOOSE : VIEWS::ROLL::SEATED});
  }
}

void placed(GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted) {
  const String board = VIEWS::ROLL::board();
  for (Whole row = 0; row < wanted.size(); ++row) {
    const String id = GUI::SAC::SEAT::named(board.c_str(), row);
    if (!GUI::SAC::CARRY::GET::dragged(page, id.c_str()))
      GUI::set(
        page, id.c_str(),
        GUI::Position{
          VIEWS::ROLL::across(wanted[row].at),
          VIEWS::ROLL::lane(wanted[row].pitch)});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{VIEWS::ROLL::wide(wanted[row].length), VIEWS::ROLL::TALL});
  }
}

}  // namespace

auto SOUND::VIEWS::ROLL::pips() -> Vector<Pip> {
  const LANE::Mark mark = LANE::edited(KIND::NOTES);
  Vector<Pip> wanted;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(mark.track, mark.lane))
    ::gathered(span, mark.track, wanted);
  return wanted;
}

void SOUND::VIEWS::ROLL::seat(
  GUI::Handle page, const Vector<Pip> &wanted, const Vector<Pip> &shown) {
  ::dressed(page, wanted, shown);
  ::placed(page, wanted);
}

auto SOUND::VIEWS::ROLL::wide(Whole length) -> Float {
  return std::max(across(length), LEAST / scaled());
}

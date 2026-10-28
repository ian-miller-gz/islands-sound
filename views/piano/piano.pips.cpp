// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/carry.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "piano.notation.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;

auto boxed(const VIEWS::ROLL::Pip &pip, Flag struck) -> VIEWS::ROLL::Box {
  const Float x = VIEWS::ROLL::across(pip.at);
  const Float lane = VIEWS::ROLL::ROWS::lane(pip.pitch, pip.at);
  if (struck)
    return {
      {x, lane + VIEWS::ROLL::LIFT}, {VIEWS::ROLL::SPACE, VIEWS::ROLL::SPACE}};
  return {{x, lane}, {VIEWS::ROLL::wide(pip.length), VIEWS::ROLL::TALL}};
}

Vector<VIEWS::ROLL::Box> laid;
Flag carried = false;
Whole born = 0;

void lay(GUI::Handle page, const String &id, const VIEWS::ROLL::Box &box) {
  if (!GUI::SAC::CARRY::GET::dragged(page, id.c_str()))
    GUI::set(page, id.c_str(), box.at);
  GUI::set(page, id.c_str(), box.size);
}

void placed(
  GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted, Whole kept) {
  const String board = VIEWS::ROLL::board();
  const Flag struck = VIEWS::ROLL::GET::staffed();
  const Flag carrying = !GUI::SAC::CARRY::GET::carried(page).empty();
  if (VIEWS::reborn(::born) || (::carried && !carrying)) ::laid.clear();
  ::carried = carrying;
  ::laid.resize(std::min(::laid.size(), kept));
  ::laid.resize(wanted.size());
  for (Whole row = 0; row < wanted.size(); ++row) {
    const VIEWS::ROLL::Box box = ::boxed(wanted[row], struck);
    if (box == ::laid[row]) continue;
    ::lay(page, GUI::SAC::SEAT::named(board.c_str(), row), box);
    ::laid[row] = box;
  }
}

}  // namespace

void SOUND::VIEWS::ROLL::gathered(
  const LANE::Clip &span, Whole track, Vector<Pip> &wanted) {
  const Inventory &pool = INVENTORY::held();
  if (span.stock >= pool.stocks.size()) return;
  const Score &score = pool.stocks[span.stock].score;
  const Whole opens = opened(span.at);
  const Whole held = opened(span.frames);
  for (Whole index = 0; index < score.notes.size(); ++index) {
    const Note &note = score.notes[index];
    if (note.at < span.from || note.at - span.from >= held) continue;
    wanted.push_back(
      {span.stock, index, note.pitch, opens + note.at - span.from, note.length,
       track});
  }
}

auto SOUND::VIEWS::ROLL::boxed(const Pip &pip) -> Box {
  return ::boxed(pip, GET::staffed());
}

auto SOUND::VIEWS::ROLL::pips() -> Vector<Pip> {
  const LANE::Mark mark = LANE::edited(KIND::NOTES);
  Vector<Pip> wanted;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(mark.track, mark.lane))
    gathered(span, mark.track, wanted);
  return wanted;
}

void SOUND::VIEWS::ROLL::seat(
  GUI::Handle page, const Vector<Pip> &wanted, const Vector<Pip> &shown) {
  dressed(page, wanted, shown);
  ::placed(page, wanted, shown.size());
  HEADS::seat(page, wanted);
  ghosts(page);
}

auto SOUND::VIEWS::ROLL::wide(Whole length) -> Float {
  return std::max(across(length), LEAST / scaled());
}

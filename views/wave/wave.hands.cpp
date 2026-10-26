// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include <island/gui/stroke.hpp>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

Whole picked = NONE;

auto under(const Vector<WAVE::Clip> &seated, Float at) -> Whole {
  for (Whole row = 0; row < seated.size(); ++row)
    if (
      at >= Float(seated[row].at) &&
      at < Float(seated[row].at + seated[row].frames))
      return row;
  return NONE;
}

auto slipped(const WAVE::Clip &clip, Float travel) -> Whole {
  const Inventory &pool = INVENTORY::held();
  if (clip.stock >= pool.stocks.size()) return clip.from;
  const Integer length = Integer(INVENTORY::length(pool.stocks[clip.stock]));
  if (length == 0) return clip.from;
  const Integer moved = Integer(std::lround(travel / clip.ratio)) % length;
  const Integer at = (Integer(clip.from % Whole(length)) - moved) % length;
  return Whole(at < 0 ? at + length : at);
}

void slip(Whole row, const WAVE::Clip &was, Float travel) {
  WAVE::Clip now = was;
  now.from = ::slipped(was, travel);
  if (now.from != was.from) WAVE::retouch(row, was, now, WAVE::SLIP);
}

}  // namespace

auto SOUND::VIEWS::WAVE::chosen() -> Whole {
  return ::picked < placed().size() ? ::picked : NONE;
}

void SOUND::VIEWS::WAVE::choose(const GUI::SAC::STROKE::Gesture &gesture) {
  if (!gesture.opened) return;
  const Whole row = ::under(placed(), gesture.from.at.x);
  if (row != NONE) ::picked = row;
}

void SOUND::VIEWS::WAVE::retouch(
  Whole row, const Clip &was, const Clip &now, STRING::Hot act) {
  const LANE::Mark mark = laned();
  if (mark.lane == NONE) return;
  if (!TIMELINE::span(mark.track, mark.lane, row, now)) return;
  HISTORY::record(
    {.act = act,
     .verb = Edit::CHANGED,
     .placements = {
       {.track = mark.track,
        .lane = mark.lane,
        .placement = row,
        .span = now,
        .was = was}}});
}

void SOUND::VIEWS::WAVE::hands() {
  const String id = board();
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  VIEWS::draw(
    {id.c_str(),
     SLIP,
     {COLUMN / scale, deep()},
     GUI::SAC::STROKE::Mode::BOARD});
  const GUI::SAC::STROKE::Gesture gesture = VIEWS::GET::gesture();
  choose(gesture);
  if (stretch(gesture)) return;
  if (!gesture.closed || !gesture.dragged || fade(gesture)) return;
  const Vector<Clip> seated = placed();
  const Whole row = ::under(seated, gesture.from.at.x);
  if (row == NONE || !LANE::GET::admitted(seated[row].at)) return;
  ::slip(row, seated[row], gesture.to.at.x - gesture.from.at.x);
}

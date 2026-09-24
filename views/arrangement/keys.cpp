// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>
#include <island/input.hpp>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole COPY = 'C';
constexpr Whole PASTE = 'V';
constexpr Whole CASED = 'a' - 'A';

auto folded(Whole code) -> Whole {
  return code >= 'a' && code <= 'z' ? code - ::CASED : code;
}

struct Copied {
  Flag full = false;
  String track, lane;
  Whole stock = 0, from = 0, frames = 0;
};

Copied carried;

auto chosen(GUI::Handle page) -> Whole {
  const Whole chose =
    GUI::SAC::SEAT::selected(page, VIEWS::ARRANGEMENT::board().c_str());
  return chose < VIEWS::ARRANGEMENT::boxes().size() ? chose : NONE;
}

void axed() { VIEWS::ARRANGEMENT::lifted(VIEWS::ARRANGEMENT::chosen()); }

void copied(GUI::Handle page) {
  const Whole chose = ::chosen(page);
  if (chose == NONE) return;
  const VIEWS::ARRANGEMENT::Box &held = VIEWS::ARRANGEMENT::boxes()[chose];
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    held.track >= tracks.size() || held.lane >= tracks[held.track].lanes.size())
    return;
  ::carried = {
    true,
    tracks[held.track].name,
    tracks[held.track].lanes[held.lane].name,
    held.stock,
    held.from,
    held.frames};
}

void pasted() {
  if (!::carried.full) return;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track) {
    if (tracks[track].name != ::carried.track) continue;
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane) {
      if (tracks[track].lanes[lane].name != ::carried.lane) continue;
      const ARRANGEMENT::TRACK::LANE::Clip span = {
        .stock = ::carried.stock,
        .at = TRANSPORT::marker().position,
        .from = ::carried.from,
        .frames = ::carried.frames};
      const Whole index = TIMELINE::place(INVENTORY::held(), track, lane, span);
      if (index == NONE) return;
      HISTORY::record(
        {.act = "clip",
         .placements = {
           {.track = track, .lane = lane, .placement = index, .span = span}}});
      return;
    }
  }
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::keys() {
  const GUI::Handle page = VIEWS::document();
  const INPUT::KEYS::Event key = GUI::GET::keyed(page);
  if (VIEWS::journaled(key)) return;
  if (key.action == INPUT::KEYS::DELETE) return ::axed();
  if (key.action != INPUT::KEYS::TEXT || !key.control) return;
  const Whole letter = ::folded(key.codepoint);
  if (letter == ::COPY) return ::copied(page);
  if (letter == ::PASTE) ::pasted();
}

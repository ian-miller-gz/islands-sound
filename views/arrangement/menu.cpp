// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../commands.hpp"
#include "../../history.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"
#include "arrangement.opened.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WHOSE = "arrangement";
constexpr STRING::Hot TRACKED = "Track";
constexpr STRING::Hot BUSSED = "Bus";
constexpr Whole BUS = 1;

struct Subject {
  Whole track = NONE, box = NONE;
};
Subject about;

auto aboard(GUI::Handle page, const GUI::NGA::Ask &ask) -> GUI::Position {
  const String id = VIEWS::ARRANGEMENT::board();
  const GUI::Position corner = GUI::GET::origin(page, id.c_str());
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id.c_str());
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, id.c_str());
  if (zoom.value <= 0.0f) return {ask.x, ask.y};
  const Float down = zoom.down > 0.0f ? zoom.down : zoom.value;
  return {
    pan.x + (ask.x - corner.x) / zoom.value, pan.y + (ask.y - corner.y) / down};
}

void took(Whole row) {
  const ::Subject held = ::about;
  ::about = {};
  if (held.box == NONE) return void(COMMANDS::bussed(held.track, row == ::BUS));
  VIEWS::ARRANGEMENT::BOX::take(held.box, row);
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::lift(Whole track, Whole lane, Whole placement)
  -> Flag {
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size())
    return false;
  const auto &clips = tracks[track].lanes[lane].clips;
  if (placement >= clips.size()) return false;
  const Placement covered = {track, lane, placement, clips[placement]};
  if (!TIMELINE::lift(track, lane, placement)) return false;
  HISTORY::record(
    {.act = "lift", .verb = Edit::REMOVED, .placements = {covered}});
  return true;
}

auto SOUND::VIEWS::ARRANGEMENT::lifted(const Vector<Whole> &rows) -> Flag {
  const Vector<Box> &laid = boxes();
  Vector<Placement> covered;
  for (const Whole row : rows) {
    if (row >= laid.size()) continue;
    const Box &box = laid[row];
    const auto &clips =
      TIMELINE::held().tracks[box.track].lanes[box.lane].clips;
    if (box.placement < clips.size())
      covered.push_back(
        {box.track, box.lane, box.placement, clips[box.placement]});
  }
  if (covered.empty()) return false;
  for (Whole back = covered.size(); back > 0; --back) {
    const Placement &one = covered[back - 1];
    if (!TIMELINE::lift(one.track, one.lane, one.placement)) return false;
  }
  HISTORY::record(
    {.act = "lift", .verb = Edit::REMOVED, .placements = covered});
  chosen({});
  return true;
}

void SOUND::VIEWS::ARRANGEMENT::menu() {
  const GUI::Handle page = VIEWS::document();
  if (::about.track != NONE) {
    const Whole row = VIEWS::MENU::taken(::WHOSE);
    if (row != NONE) return ::took(row);
    if (!VIEWS::MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  const GUI::Position at = ::aboard(page, ask);
  const Whole box = boxed(at.x, at.y);
  if (box != NONE) {
    ::about = {boxes()[box].track, box};
    return VIEWS::MENU::raise(BOX::labels(box, at.x), ask.x, ask.y, ::WHOSE);
  }
  const Whole track = ribboned(at.y);
  if (track == NONE) return;
  ::about = {.track = track};
  VIEWS::MENU::raise(
    {String(::TRACKED), String(::BUSSED)}, ask.x, ask.y, ::WHOSE);
}
